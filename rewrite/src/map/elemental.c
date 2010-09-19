// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "../common/cbasetypes.h"
#include "../common/malloc.h"
#include "../common/socket.h"
#include "../common/timer.h"
#include "../common/nullpo.h"
#include "../common/mmo.h"
#include "../common/showmsg.h"
#include "../common/utils.h"

#include "log.h"
#include "clif.h"
#include "chrif.h"
#include "intif.h"
#include "itemdb.h"
#include "map.h"
#include "pc.h"
#include "status.h"
#include "skill.h"
#include "mob.h"
#include "pet.h"
#include "battle.h"
#include "party.h"
#include "guild.h"
#include "atcommand.h"
#include "script.h"
#include "npc.h"
#include "trade.h"
#include "unit.h"
#include "elemental.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct s_elemental_db elemental_db[MAX_ELEMENTAL_CLASS]; // Elemental Database

int ele_search_index(int class_)
{
	int i;
	ARR_FIND(0, MAX_ELEMENTAL_CLASS, i, elemental_db[i].class_ == class_);
	return (i == MAX_ELEMENTAL_CLASS)?-1:i;
}

bool ele_class(int class_)
{
	return (bool)(ele_search_index(class_) > -1);
}

struct view_data * ele_get_viewdata(int class_)
{
	int i = ele_search_index(class_);
	if( i < 0 )
		return 0;

	return &elemental_db[i].vd;
}

int ele_create(struct map_session_data *sd, int class_, unsigned int lifetime)
{
	struct s_elemental ele;
	struct s_elemental_db *db;
	int i;

	nullpo_retr(1,sd);

	if( (i = ele_search_index(class_)) < 0 )
		return 0;

	db = &elemental_db[i];
	memset(&ele,0,sizeof(struct s_elemental));

	ele.char_id = sd->status.char_id;
	ele.class_ = class_;
	ele.mode = MD_CANMOVE; // Initial mode
	ele.hp = db->status.max_hp;
	ele.sp = db->status.max_sp;
	ele.life_time = lifetime;

	// Request Char Server to create this elemental
	intif_elemental_create(&ele);

	return 1;
}

int elemental_get_lifetime(struct elemental_data *ed)
{
	const struct TimerData * td;
	if( ed == NULL || ed->summon_timer == INVALID_TIMER )
		return 0;

	td = get_timer(ed->summon_timer);
	return (td != NULL) ? DIFF_TICK(td->tick, gettick()) : 0;
}

int elemental_save(struct elemental_data *ed)
{
	ed->elemental.hp = ed->battle_status.hp;
	ed->elemental.sp = ed->battle_status.sp;
	ed->elemental.life_time = elemental_get_lifetime(ed);

	intif_elemental_save(&ed->elemental);
	return 1;
}

static int ele_summon_end(int tid, unsigned int tick, int id, intptr data)
{
	struct map_session_data *sd;
	struct elemental_data *ed;

	if( (sd = map_id2sd(id)) == NULL )
		return 1;
	if( (ed = sd->ed) == NULL )
		return 1;

	if( ed->summon_timer != tid )
	{
		ShowError("ele_summon_end %d != %d.\n", ed->summon_timer, tid);
		return 0;
	}

	ed->summon_timer = INVALID_TIMER;
	ele_delete(ed, 0); // Elemental's summon time is over.

	return 0;
}

int ele_delete(struct elemental_data *ed, int reply)
{
	struct map_session_data *sd = ed->master;
	ed->elemental.life_time = 0;
	ed->summon_timer = INVALID_TIMER;

	ele_summon_stop(ed);

	if( !sd )
		return unit_free(&ed->bl, 0);

	//TODO: send a message to the master.

	return unit_remove_map(&ed->bl, 0);
}

void ele_summon_stop(struct elemental_data *ed)
{
	nullpo_retv(ed);
	if( ed->summon_timer != INVALID_TIMER )
		delete_timer(ed->summon_timer, ele_summon_end);
	ed->summon_timer = INVALID_TIMER;
}

void ele_summon_init(struct elemental_data *ed)
{
	if( ed->summon_timer == INVALID_TIMER )
		ed->summon_timer = add_timer(gettick() + ed->elemental.life_time, ele_summon_end, ed->master->bl.id, 0);

	ed->regen.state.block = 0;
}

int ele_data_received(struct s_elemental *ele, bool flag)
{
	struct map_session_data *sd;
	struct elemental_data *ed;
	struct s_elemental_db *db;
	int i = ele_search_index(ele->class_);

	if( (sd = map_charid2sd(ele->char_id)) == NULL )
		return 0;

	if( !flag || i < 0 )
	{ // Not created - loaded - DB info
		sd->status.ele_id = 0;
		return 0;
	}

	db = &elemental_db[i];
	if( !sd->ed )
	{	// Initialize it after first summon.
		sd->ed = ed = (struct elemental_data*)aCalloc(1,sizeof(struct elemental_data));
		ed->bl.type = BL_ELEM;
		ed->bl.id = npc_get_new_npc_id();
		ed->master = sd;
		ed->db = db;
		memcpy(&ed->elemental, ele, sizeof(struct s_elemental));
		status_set_viewdata(&ed->bl, ed->elemental.class_);
		ed->vd->head_mid = 10; // Why?
		status_change_init(&ed->bl);
		unit_dataset(&ed->bl);
		ed->ud.dir = sd->ud.dir;

		ed->bl.m = sd->bl.m;
		ed->bl.x = sd->bl.x;
		ed->bl.y = sd->bl.y;
		unit_calc_pos(&ed->bl, sd->bl.x, sd->bl.y, sd->ud.dir);
		ed->bl.x = ed->ud.to_x;
		ed->bl.y = ed->ud.to_y;

		ed->base_status.max_hp += 500 * pc_checkskill(sd,SO_EL_SYMPATHY);
		ed->base_status.max_sp += 50 * pc_checkskill(sd,SO_EL_SYMPATHY);

		//TODO: Set elemental status.

		map_addiddb(&ed->bl);
		status_calc_elemental(ed,1);
		ed->last_thinktime = gettick();
		ed->summon_timer = INVALID_TIMER;
		ed->battle_status.mode = ele->mode = EL_MODE_PASSIVE; // Initial mode.
		ele_summon_init(ed);
	}
	else
	{
		memcpy(&sd->ed->elemental, ele, sizeof(struct s_elemental));
		ed = sd->ed;
	}

	sd->status.ele_id = ele->elemental_id;

	if( ed && ed->bl.prev == NULL && sd->bl.prev != NULL )
	{
		map_addblock(&ed->bl);
		clif_spawn(&ed->bl);
		clif_elemental_info(sd);
		clif_elemental_updatestatus(sd,SP_HP);
		clif_hpmeter_single(sd->fd,ed->bl.id,ed->battle_status.hp,ed->battle_status.matk_max);
		clif_elemental_updatestatus(sd,SP_SP);
	}

	return 1;
}

/*===============================================================
 * Action that elemental perform after changing mode.
 * Activates one of the skills of the new mode.
 *-------------------------------------------------------------*/
int elemental_change_mode_ack(struct elemental_data *ed, int mode)
{
	nullpo_retr(0,ed);
	return 1;
}

/*===============================================================
 * Change elemental mode.
 *-------------------------------------------------------------*/
int elemental_change_mode(struct elemental_data *ed, int mode)
{
	nullpo_retr(0,ed);

	// Remove target
	elemental_unlocktarget(ed);
	
	ed->battle_status.mode = ed->elemental.mode = mode;
	//TODO: elemental_changemode_ack(ed,mode);
	return 1;
}

void elemental_damage(struct elemental_data *ed, struct block_list *src, int hp, int sp)
{
	if( hp )
		clif_elemental_updatestatus(ed->master, SP_HP);
	if( sp )
		clif_elemental_updatestatus(ed->master, SP_SP);
}

void elemental_heal(struct elemental_data *ed, int hp, int sp)
{
	if( hp )
		clif_elemental_updatestatus(ed->master, SP_HP);
	if( sp )
		clif_elemental_updatestatus(ed->master, SP_SP);
}

int elemental_dead(struct elemental_data *ed, struct block_list *src)
{
	ele_delete(ed, 1);
	return 0;
}

int elemental_unlocktarget(struct elemental_data *ed)
{
	nullpo_retr(0, ed);

	ed->target_id = 0;
	elemental_stop_attack(ed);
	elemental_stop_walking(ed,1);
	return 0;
}

int elemental_set_target( struct map_session_data *sd, struct block_list *bl )
{
	struct elemental_data *ed = sd->ed;

	nullpo_retr(0,ed);
	nullpo_retr(0,bl);

	if( ed->bl.m != bl->m || !check_distance_bl(&ed->bl, bl, ed->db->range2) )
		return 0;
	
	if( !status_check_skilluse(&ed->bl, bl, 0, 0, 0) )
		return 0;

	if( ed->target_id == 0 )
		ed->target_id = bl->id;

	return 1;
}

static int elemental_ai_sub_timer_activesearch(struct block_list *bl, va_list ap)
{
	struct elemental_data *ed;
	struct block_list **target;
	int dist;

	nullpo_retr(0, bl);

	ed = va_arg(ap,struct elemental_data *);
	target = va_arg(ap,struct block_list**);

	//If can't seek yet, not an enemy, or you can't attack it, skip.
	if( (*target) == bl || !status_check_skilluse(&ed->bl, bl, 0, 0, 0) )
		return 0;

	if( battle_check_target(&ed->bl,bl,BCT_ENEMY) <= 0 )
		return 0;

	switch( bl->type )
	{
		case BL_PC:
			if( !map_flag_vs(ed->bl.m) )
				return 0;
		default:
			dist = distance_bl(&ed->bl, bl);
			if( ((*target) == NULL || !check_distance_bl(&ed->bl, *target, dist)) && battle_check_range(&ed->bl,bl,ed->db->range2) )
			{ //Pick closest target?
				(*target) = bl;
				ed->target_id = bl->id;
				ed->min_chase = dist + ed->db->range3;
				if( ed->min_chase > AREA_SIZE )
					ed->min_chase = AREA_SIZE;
				return 1;
			}
			break;
	}
	return 0;
}

static int elemental_ai_sub_timer(struct elemental_data *ed, struct map_session_data *sd, unsigned int tick)
{
	struct block_list *target = NULL, *abl = NULL;
	struct block_list *mtbl = NULL;
	int master_dist, view_range, mode;

	nullpo_retr(0,ed);
	nullpo_retr(0,sd);

	if( ed->bl.prev == NULL || sd == NULL || sd->bl.prev == NULL )
		return 0;

	if( DIFF_TICK(tick,ed->last_thinktime) < MIN_ELETHINKTIME )
		return 0;

	ed->last_thinktime = tick;

	if( ed->ud.skilltimer != -1 )
		return 0;

	if( ed->ud.walktimer != -1 && ed->ud.walkpath.path_pos <= 2 )
		return 0; //No thinking when you just started to walk.

	if( ed->sc.count && ed->sc.data[SC_BLIND] )
		view_range = 3;
	else
		view_range = ed->db->range2;

	mode = status_get_mode(&ed->bl);
	
	master_dist = distance_bl(&sd->bl, &ed->bl);
	if( master_dist > AREA_SIZE )
	{	// Master out of vision range.
		unit_warp(&ed->bl,sd->bl.m,sd->bl.x,sd->bl.y,3);
		return 0;
	}
	else if( master_dist > MAX_ELEDISTANCE )
	{	// Master too far, chase.
		short x = sd->bl.x, y = sd->bl.y;
		if( ed->target_id )
			elemental_unlocktarget(ed);
		if( ed->ud.walktimer != -1 && ed->ud.target == sd->bl.id )
			return 0; //Already walking to him
		if( DIFF_TICK(tick, ed->ud.canmove_tick) < 0 )
			return 0; //Can't move yet.
		if( map_search_freecell(&ed->bl, sd->bl.m, &x, &y, MIN_ELEDISTANCE, MIN_ELEDISTANCE, 1) 
			&& unit_walktoxy(&ed->bl, x, y, 0) )
		return 0;
	}

	// Choose an action based on elemental mode
	if( mode == EL_MODE_ASSIST )
	{
		if( ed->target_id )
		{
			target = map_id2bl(ed->target_id);

			if( sd->ud.target && sd->ud.state.attack_continue )
				mtbl = map_id2bl(sd->ud.target);
			else if( sd->ud.skilltarget )
			{
				mtbl = map_id2bl(sd->ud.target);
				if( mtbl && battle_check_target(&ed->bl, mtbl, BCT_ENEMY) <= 0 )
					mtbl = NULL;
			}

			if( !target || ( mtbl && mtbl->id != target->id) )
			{
				elemental_unlocktarget(ed);
				return 0;
			}

			//Attempt to attack.
			//At this point we know the target is attackable, we just gotta check if the range matches.
			if( ed->ud.target == target->id && ed->ud.attacktimer != -1 ) //Already locked.
				return 1;
	
			if( battle_check_range(&ed->bl, target, ed->base_status.rhw.range) )
			{	//Target within range, engage
				unit_attack(&ed->bl,target->id,1);
				return 1;
			}

			//Follow up if possible.
			else if( !unit_walktobl(&ed->bl, target, ed->base_status.rhw.range, 2) )
				elemental_unlocktarget(ed);

		}
		else
		{
			if( DIFF_TICK(ed->last_linktime, tick) >= MIN_MOBLINKTIME )
				return 0;
			
			if( sd->ud.target && sd->ud.state.attack_continue )
				mtbl = map_id2bl(sd->ud.target);
			else if( sd->ud.skilltarget )
			{
				mtbl = map_id2bl(sd->ud.target);
				if( mtbl && battle_check_target(&sd->bl, mtbl, BCT_ENEMY) <= 0 )
					mtbl = NULL;
			}

			if( mtbl )
				ed->target_id = mtbl->id;
		}
	}
	else if( mode == EL_MODE_AGGRESSIVE )
	{
		target = map_id2bl(ed->ud.target);

		if( !target )
			map_foreachinrange(elemental_ai_sub_timer_activesearch, &ed->bl, ed->base_status.rhw.range, BL_CHAR, ed, &target, status_get_mode(&ed->bl));

		if( !target )
		{ //No targets available.
			elemental_unlocktarget(ed);
			return 1;
		}
	
		//Attempt to attack.
		//At this point we know the target is attackable, we just gotta check if the range matches.
		if( ed->ud.target == target->id && ed->ud.attacktimer != -1 ) //Already locked.
			return 1;
	
		if( battle_check_range(&ed->bl, target, ed->base_status.rhw.range) )
		{	//Target within range, engage
			unit_attack(&ed->bl,target->id,1);
			return 1;
		}

		//Follow up if possible.
		if( !unit_walktobl(&ed->bl, target, ed->base_status.rhw.range, 2) )
			elemental_unlocktarget(ed);
	}

	return 0;
}

static int elemental_ai_sub_foreachclient(struct map_session_data *sd, va_list ap)
{
	unsigned int tick = va_arg(ap,unsigned int);
	if(sd->status.ele_id && sd->ed)
		elemental_ai_sub_timer(sd->ed,sd,tick);

	return 0;
}

static int elemental_ai_timer(int tid, unsigned int tick, int id, intptr data)
{
	map_foreachpc(elemental_ai_sub_foreachclient,tick);

	return 0;
}

int read_elementaldb(void)
{
	FILE *fp;
	char line[1024], *p;
	char *str[26];
	int i, j = 0, k = 0, ele;
	struct s_elemental_db *db;
	struct status_data *status;

	sprintf(line, "%s/%s", db_path, "elemental_db.txt");
	memset(elemental_db,0,sizeof(elemental_db));

	fp = fopen(line, "r");
	if( !fp )
	{
		ShowError("read_elementaldb : can't read elemental_db.txt\n");
		return -1;
	}

	while( fgets(line, sizeof(line), fp) && j < MAX_ELEMENTAL_CLASS )
	{
		k++;
		if( line[0] == '/' && line[1] == '/' )
			continue;

		i = 0;
		p = strtok(line, ",");
		while( p != NULL && i < 26 )
		{
			str[i++] = p;
			p = strtok(NULL, ",");
		}
		if( i < 26 )
		{
			ShowError("read_elementaldb : Incorrect number of columns at elemental_db.txt line %d.\n", k);
			continue;
		}

		db = &elemental_db[j];
		db->class_ = atoi(str[0]);
		strncpy(db->sprite, str[1], NAME_LENGTH);
		strncpy(db->name, str[2], NAME_LENGTH);
		db->lv = atoi(str[3]);

		status = &db->status;
		db->vd.class_ = db->class_;

		status->max_hp = atoi(str[4]);
		status->max_sp = atoi(str[5]);
		status->rhw.range = atoi(str[6]);
		status->rhw.atk = atoi(str[7]);
		status->rhw.atk2 = status->rhw.atk + atoi(str[8]);
		status->def = atoi(str[9]);
		status->mdef = atoi(str[10]);
		status->str = atoi(str[11]);
		status->agi = atoi(str[12]);
		status->vit = atoi(str[13]);
		status->int_ = atoi(str[14]);
		status->dex = atoi(str[15]);
		status->luk = atoi(str[16]);
		db->range2 = atoi(str[17]);
		db->range3 = atoi(str[18]);
		status->size = atoi(str[19]);
		status->race = atoi(str[20]);
	
		ele = atoi(str[21]);
		status->def_ele = ele%10;
		status->ele_lv = ele/20;
		if( status->def_ele >= ELE_MAX )
		{
			ShowWarning("Elemental %d has invalid element type %d (max element is %d)\n", db->class_, status->def_ele, ELE_MAX - 1);
			status->def_ele = ELE_NEUTRAL;
		}
		if( status->ele_lv < 1 || status->ele_lv > 4 )
		{
			ShowWarning("Elemental %d has invalid element level %d (max is 4)\n", db->class_, status->ele_lv);
			status->ele_lv = 1;
		}

		status->aspd_rate = 1000;
		status->speed = atoi(str[22]);
		status->adelay = atoi(str[23]);
		status->amotion = atoi(str[24]);
		status->dmotion = atoi(str[25]);

		j++;
	}

	fclose(fp);
	ShowStatus("Done reading '"CL_WHITE"%d"CL_RESET"' elementals in '"CL_WHITE"db/elemental_db.txt"CL_RESET"'.\n",j);

	return 0;
}

int read_elemental_skilldb(void)
{
	FILE *fp;
	char line[1024], *p;
	char *str[4];
	struct s_elemental_db *db;
	int i, j = 0, k = 0, class_;
	int skillid, skilllv, skillmode;

	sprintf(line, "%s/%s", db_path, "elemental_skill_db.txt");
	fp = fopen(line, "r");
	if( !fp )
	{
		ShowError("read_elemental_skilldb : can't read elemental_skill_db.txt\n");
		return -1;
	}

	while( fgets(line, sizeof(line), fp) )
	{
		k++;
		if( line[0] == '/' && line[1] == '/' )
			continue;

		i = 0;
		p = strtok(line, ",");
		while( p != NULL && i < 4 )
		{
			str[i++] = p;
			p = strtok(NULL, ",");
		}
		if( i < 4 )
		{
			ShowError("read_elemental_skilldb : Incorrect number of columns at elemental_skill_db.txt line %d.\n", k);
			continue;
		}

		class_ = atoi(str[0]);
		ARR_FIND(0, MAX_ELEMENTAL_CLASS, i, class_ == elemental_db[i].class_);
		if( i == MAX_ELEMENTAL_CLASS )
		{
			ShowError("read_elemental_skilldb : Class not found in elemental_db for skill entry, line %d.\n", k);
			continue;
		}
		
		skillid = atoi(str[1]);
		if( skillid < EL_SKILLBASE || skillid >= EL_SKILLBASE + MAX_ELEMENTALSKILL )
		{
			ShowError("read_elemental_skilldb : Skill out of range, line %d.\n", k);
			continue;
		}

		db = &elemental_db[i];
		skilllv = atoi(str[2]);

		skillmode = atoi(str[3]);
		if( skillmode < EL_SKILLMODE_PASIVE || skillmode > EL_SKILLMODE_AGGRESSIVE )
		{
			ShowError("read_elemental_skilldb : Skillmode out of range, line %d.\n",k);
			skillmode = EL_SKILLMODE_PASIVE;
			continue;
		}
		ARR_FIND( 0, MAX_ELESKILLTREE, i, db->skill[i].id == 0 || db->skill[i].id == skillid );
		if( i == MAX_ELESKILLTREE )
		{
			ShowWarning("Unable to load skill %d into Elemental %d's tree. Maximum number of skills per elemental has been reached.\n", skillid, class_);
			continue;
		}
		db->skill[i].id = skillid;
		db->skill[i].lv = skilllv;
		db->skill[i].mode = skillmode;
		j++;
	}

	fclose(fp);
	ShowStatus("Done reading '"CL_WHITE"%d"CL_RESET"' entries in '"CL_WHITE"db/elemental_skill_db.txt"CL_RESET"'.\n",j);
	return 0;
}

void read_eledb(void)
{
	read_elementaldb();
}

void ele_skill_reload(void)
{
	read_elemental_skilldb();
}

int do_init_elemental(void)
{
	read_elementaldb();
	read_elemental_skilldb();

	add_timer_func_list(elemental_ai_timer,"elemental_ai_timer");
	add_timer_interval(gettick()+MIN_ELETHINKTIME,elemental_ai_timer,0,0,MIN_ELETHINKTIME);
	
	return 0;
}

int do_final_elemental(void);
