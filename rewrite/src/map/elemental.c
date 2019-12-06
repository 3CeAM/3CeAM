// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "../common/cbasetypes.h"
#include "../common/malloc.h"
#include "../common/socket.h"
#include "../common/timer.h"
#include "../common/nullpo.h"
#include "../common/mmo.h"
#include "../common/showmsg.h"
#include "../common/strlib.h"
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

int elem_search_index(int class_)
{
	int i;
	ARR_FIND(0, MAX_ELEMENTAL_CLASS, i, elemental_db[i].class_ == class_);
	return (i == MAX_ELEMENTAL_CLASS)?-1:i;
}

bool elem_class(int class_)
{
	return (bool)(elem_search_index(class_) > -1);
}

struct view_data * elem_get_viewdata(int class_)
{
	int i = elem_search_index(class_);
	if( i < 0 )
		return 0;

	return &elemental_db[i].vd;
}

int elem_create(struct map_session_data *sd, int class_, unsigned int lifetime)
{
	struct s_elemental elem;
	struct s_elemental_db *db;
	int i;
	nullpo_retr(1,sd);

	if( (i = elem_search_index(class_)) < 0 )
		return 0;

	db = &elemental_db[i];
	memset(&elem,0,sizeof(struct s_elemental));

	elem.char_id = sd->status.char_id;
	elem.class_ = class_;
	elem.hp = db->status.max_hp;
	elem.sp = db->status.max_sp;
	elem.life_time = lifetime;

#ifdef TXT_ONLY// Bypass the char server for TXT.
	elem_data_received(&elem,1);
#else
	// Request Char Server to create this elemental
	intif_elemental_create(&elem);
#endif

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

int elemental_get_type(struct elemental_data *ed)
{
	int class_;

	if( ed == NULL || ed->db == NULL )
		return -1;

	class_ = ed->db->class_;

	if( class_ >= MOBID_EL_AGNI_S && class_ <= MOBID_EL_AGNI_L )
		return ELEMTYPE_AGNI;
	if( class_ >= MOBID_EL_AQUA_S && class_ <= MOBID_EL_AQUA_L )
		return ELEMTYPE_AQUA;
	if( class_ >= MOBID_EL_VENTUS_S && class_ <= MOBID_EL_VENTUS_L )
		return ELEMTYPE_VENTUS;
	if( class_ >= MOBID_EL_TERA_S && class_ <= MOBID_EL_TERA_L )
		return ELEMTYPE_TERA;

	return -1;
}

int elemental_save(struct elemental_data *ed)
{
	ed->elemental.hp = ed->battle_status.hp;
	ed->elemental.sp = ed->battle_status.sp;
	ed->elemental.life_time = elemental_get_lifetime(ed);

#ifndef TXT_ONLY
	intif_elemental_save(&ed->elemental);
#endif
	return 1;
}

static int elem_summon_end(int tid, unsigned int tick, int id, intptr data)
{
	struct map_session_data *sd;
	struct elemental_data *ed;

	if( (sd = map_id2sd(id)) == NULL )
		return 1;
	if( (ed = sd->ed) == NULL )
		return 1;

	if( ed->summon_timer != tid )
	{
		ShowError("elem_summon_end %d != %d.\n", ed->summon_timer, tid);
		return 0;
	}

	ed->summon_timer = INVALID_TIMER;
	elem_delete(ed, 0); // Elemental summon time is over.

	return 0;
}

int elem_delete(struct elemental_data *ed, int reply)
{
	struct map_session_data *sd = ed->master;
	ed->elemental.life_time = 0;

	elem_summon_stop(ed);

	if( !sd )
		return unit_free(&ed->bl, CLR_OUTSIGHT);

	if( ed->water_screen_flag )
	{
		ed->water_screen_flag = 0;
		status_change_end(&sd->bl, SC_WATER_SCREEN, INVALID_TIMER);
	}

	return unit_remove_map(&ed->bl, CLR_OUTSIGHT);
}

void elem_summon_stop(struct elemental_data *ed)
{
	nullpo_retv(ed);
	if( ed->summon_timer != INVALID_TIMER )
		delete_timer(ed->summon_timer, elem_summon_end);
	ed->summon_timer = INVALID_TIMER;
}

void elem_summon_init(struct elemental_data *ed)
{
	if( ed->summon_timer == INVALID_TIMER )
		ed->summon_timer = add_timer(gettick() + ed->elemental.life_time, elem_summon_end, ed->master->bl.id, 0);

	ed->regen.state.block = 0;
}

int elem_data_received(struct s_elemental *elem, bool flag)
{
	struct map_session_data *sd;
	struct elemental_data *ed;
	struct s_elemental_db *db;
	int i = elem_search_index(elem->class_);

	if( (sd = map_charid2sd(elem->char_id)) == NULL )
		return 0;
	if( !flag || i < 0 )
	{ // Not created - loaded - DB info
		sd->status.ele_id = 0;
		return 0;
	}

	db = &elemental_db[i];
	if( !sd->ed )
	{
		sd->ed = ed = (struct elemental_data*)aCalloc(1,sizeof(struct elemental_data));
		ed->bl.type = BL_ELEM;
		ed->bl.id = npc_get_new_npc_id();
		ed->water_screen_flag = 0;

		ed->master = sd;
		ed->db = db;
		memcpy(&ed->elemental, elem, sizeof(struct s_elemental));
		status_set_viewdata(&ed->bl, ed->elemental.class_);
		status_change_init(&ed->bl);
		unit_dataset(&ed->bl);
		ed->ud.dir = sd->ud.dir;

		ed->bl.m = sd->bl.m;
		ed->bl.x = sd->bl.x;
		ed->bl.y = sd->bl.y;
		unit_calc_pos(&ed->bl, sd->bl.x, sd->bl.y, sd->ud.dir);
		ed->bl.x = ed->ud.to_x;
		ed->bl.y = ed->ud.to_y;

		map_addiddb(&ed->bl);
		status_calc_elemental(ed,1);
		ed->summon_timer = INVALID_TIMER;
		elem_summon_init(ed);
	}
	else
	{
		memcpy(&sd->ed->elemental, elem, sizeof(struct s_elemental));
		ed = sd->ed;
	}

	//if( sd->status.ele_id == 0 )
	//	elemental_set_calls(ed, 1);
	sd->status.ele_id = elem->elemental_id;

	if( ed && ed->bl.prev == NULL && sd->bl.prev != NULL )
	{
		map_addblock(&ed->bl);
		clif_spawn(&ed->bl);
		clif_elemental_info(sd);
	}

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
	elem_delete(ed, 1);
	return 0;
}

int elemental_checkskill(struct elemental_data *ed, int skill_id)
{
	int i = skill_id - EL_SKILLBASE;

	if( !ed || !ed->db )
		return 0;
	if( ed->db->skill[i].id == skill_id )
		return ed->db->skill[i].lv;

	return 0;
}

static bool read_elementaldb_sub(char* str[], int columns, int current)
{
	int ele;
	struct s_elemental_db *db;
	struct status_data *status;

	db = &elemental_db[current];
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

	status->aspd_amount = 0;
	status->aspd_rate = 1000;
	status->speed = atoi(str[22]);
	status->adelay = atoi(str[23]);
	status->amotion = atoi(str[24]);
	status->dmotion = atoi(str[25]);

	return true;
}

int read_elementaldb(void)
{
	memset(elemental_db,0,sizeof(elemental_db));
	sv_readdb(db_path, "elemental_db.txt", ',', 26, 26, MAX_ELEMENTAL_CLASS, &read_elementaldb_sub);

	return 0;
}

static bool read_elemental_skilldb_sub(char* str[], int columns, int current)
{// <elem id>,<skill id>,<elem mode>
	struct s_elemental_db *db;
	int i, class_;
	int skillid, mode;

	class_ = atoi(str[0]);
	ARR_FIND(0, MAX_ELEMENTAL_CLASS, i, class_ == elemental_db[i].class_);
	if( i == MAX_ELEMENTAL_CLASS )
	{
		ShowError("read_elemental_skilldb : Class %d not found in elemental_db for skill entry.\n", class_);
		return false;
	}
	
	skillid = atoi(str[1]);
	if( skillid < EL_SKILLBASE || skillid >= EL_SKILLBASE + MAX_ELEMSKILL )
	{
		ShowError("read_elemental_skilldb : Skill %d out of range.\n", skillid);
		return false;
	}

	db = &elemental_db[i];

	mode = atoi(str[2]);
	if( mode < ELEMMODE_PASSIVE || mode > ELEMMODE_OFFENSIVE )
	{
		ShowError("read_elemental_skilldb : Mode %d out of range.\n", mode);
		return false;
	}

	i = skillid - EL_SKILLBASE;
	db->skill[i].id = skillid;
	db->skill[i].lv = 1;// All elemental skills have a max level of 1.
	db->skill[i].mode = mode;

	return true;
}

int read_elemental_skilldb(void)
{
	sv_readdb(db_path, "elemental_skill_db.txt", ',', 3, 3, -1, &read_elemental_skilldb_sub);

	return 0;
}

int do_init_elemental(void)
{
	read_elementaldb();
	read_elemental_skilldb();
	
	//add_timer_func_list(elemental_summon, "elemental_summon");
	return 0;
}

int do_final_elemental(void);
