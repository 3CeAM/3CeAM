// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "../common/cbasetypes.h"
#include "../common/timer.h"
#include "../common/nullpo.h"
#include "../common/malloc.h"
#include "../common/showmsg.h"
#include "../common/ers.h"
#include "../common/strlib.h"
#include "../common/utils.h"

#include "map.h"
#include "path.h"
#include "pc.h"
#include "status.h"
#include "skill.h"
#include "homunculus.h"
#include "mercenary.h"
#include "elemental.h"
#include "mob.h"
#include "itemdb.h"
#include "clif.h"
#include "pet.h"
#include "guild.h"
#include "party.h"
#include "battle.h"
#include "battleground.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int attr_fix_table[4][ELE_MAX][ELE_MAX];

struct Battle_Config battle_config;
static struct eri *delay_damage_ers; //For battle delay damage structures.

int battle_getcurrentskill(struct block_list *bl)
{	//Returns the current/last skill in use by this bl.
	struct unit_data *ud;

	if( bl->type == BL_SKILL )
	{
		struct skill_unit * su = (struct skill_unit*)bl;
		return su->group?su->group->skill_id:0;
	}
	ud = unit_bl2ud(bl);
	return ud?ud->skillid:0;
}

/*==========================================
 * Get random targetting enemy
 *------------------------------------------*/
static int battle_gettargeted_sub(struct block_list *bl, va_list ap)
{
	struct block_list **bl_list;
	struct unit_data *ud;
	int target_id;
	int *c;

	bl_list = va_arg(ap, struct block_list **);
	c = va_arg(ap, int *);
	target_id = va_arg(ap, int);

	if (bl->id == target_id)
		return 0;
	if (*c >= 24)
		return 0;

	ud = unit_bl2ud(bl);
	if (!ud) return 0;

	if (ud->target == target_id || ud->skilltarget == target_id) {
		bl_list[(*c)++] = bl;
		return 1;
	}
	return 0;	
}

struct block_list* battle_gettargeted(struct block_list *target)
{
	struct block_list *bl_list[24];
	int c = 0;
	nullpo_retr(NULL, target);

	memset(bl_list, 0, sizeof(bl_list));
	map_foreachinrange(battle_gettargeted_sub, target, AREA_SIZE, BL_CHAR, bl_list, &c, target->id);
	if (c == 0 || c > 24)
		return NULL;
	return bl_list[rand()%c];
}


//Returns the id of the current targetted character of the passed bl. [Skotlex]
int battle_gettarget(struct block_list* bl)
{
	switch (bl->type)
	{
		case BL_PC:  return ((struct map_session_data*)bl)->ud.target;
		case BL_MOB: return ((struct mob_data*)bl)->target_id;
		case BL_PET: return ((struct pet_data*)bl)->target_id;
		case BL_HOM: return ((struct homun_data*)bl)->ud.target;
		case BL_MER: return ((struct mercenary_data*)bl)->ud.target;
		case BL_ELEM: return ((struct elemental_data*)bl)->ud.target;
	}
	return 0;
}

static int battle_getenemy_sub(struct block_list *bl, va_list ap)
{
	struct block_list **bl_list;
	struct block_list *target;
	int *c;

	bl_list = va_arg(ap, struct block_list **);
	c = va_arg(ap, int *);
	target = va_arg(ap, struct block_list *);

	if (bl->id == target->id)
		return 0;
	if (*c >= 24)
		return 0;
	if (status_isdead(bl))
		return 0;
	if (battle_check_target(target, bl, BCT_ENEMY) > 0) {
		bl_list[(*c)++] = bl;
		return 1;
	}
	return 0;	
}

// Picks a random enemy of the given type (BL_PC, BL_CHAR, etc) within the range given. [Skotlex]
struct block_list* battle_getenemy(struct block_list *target, int type, int range)
{
	struct block_list *bl_list[24];
	int c = 0;
	memset(bl_list, 0, sizeof(bl_list));
	map_foreachinrange(battle_getenemy_sub, target, range, type, bl_list, &c, target);
	if (c == 0 || c > 24)
		return NULL;
	return bl_list[rand()%c];
}

static int battle_getenemyarea_sub(struct block_list *bl, va_list ap)
{
	struct block_list **bl_list, *src;
	int *c, ignore_id;

	bl_list = va_arg(ap, struct block_list **);
	c = va_arg(ap, int *);
	src = va_arg(ap, struct block_list *);
	ignore_id = va_arg(ap, int);

	if( bl->id == src->id || bl->id == ignore_id )
		return 0; // Ignores Caster and a possible pre-target
	if( *c >= 24 )
		return 0;
	if( status_isdead(bl) )
		return 0;
	if( battle_check_target(src, bl, BCT_ENEMY) > 0 )
	{ // Is Enemy!...
		bl_list[(*c)++] = bl;
		return 1;
	}
	return 0;	
}

// Pick a random enemy
struct block_list* battle_getenemyarea(struct block_list *src, int x, int y, int range, int type, int ignore_id)
{
	struct block_list *bl_list[24];
	int c = 0;
	memset(bl_list, 0, sizeof(bl_list));
	map_foreachinarea(battle_getenemyarea_sub, src->m, x - range, y - range, x + range, y + range, type, bl_list, &c, src, ignore_id);
	if( c == 0 || c > 24 )
		return NULL;
	return bl_list[rand()%c];
}

// �_??[�W�̒x��
struct delay_damage {
	struct block_list *src;
	int target;
	int damage;
	int delay;
	unsigned short distance;
	unsigned short skill_lv;
	unsigned short skill_id;
	enum damage_lv dmg_lv;
	unsigned short attack_type;
};

int battle_delay_damage_sub(int tid, unsigned int tick, int id, intptr data)
{
	struct delay_damage *dat = (struct delay_damage *)data;
	struct block_list *target = map_id2bl(dat->target);
	if (target && dat && map_id2bl(id) == dat->src && target->prev != NULL && !status_isdead(target) &&
		target->m == dat->src->m &&
		(target->type != BL_PC || ((TBL_PC*)target)->invincible_timer == INVALID_TIMER) &&
		check_distance_bl(dat->src, target, dat->distance)) //Check to see if you haven't teleported. [Skotlex]
	{
		map_freeblock_lock();
		status_fix_damage(dat->src, target, dat->damage, dat->delay);
		if( dat->attack_type && !status_isdead(target) )
			skill_additional_effect(dat->src,target,dat->skill_id,dat->skill_lv,dat->attack_type,dat->dmg_lv,tick);
		if( dat->dmg_lv > ATK_BLOCK && dat->attack_type )
			skill_counter_additional_effect(dat->src,target,dat->skill_id,dat->skill_lv,dat->attack_type,tick);
		map_freeblock_unlock();
	}
	ers_free(delay_damage_ers, dat);
	return 0;
}

int battle_delay_damage (unsigned int tick, int amotion, struct block_list *src, struct block_list *target, int attack_type, int skill_id, int skill_lv, int damage, enum damage_lv dmg_lv, int ddelay)
{
	struct delay_damage *dat;
	struct status_change *sc;
	nullpo_ret(src);
	nullpo_ret(target);

	sc = status_get_sc(target);
	if( sc && sc->data[SC_DEVOTION] && damage > 0 )
		damage = 0;

	if (!battle_config.delay_battle_damage) {
		map_freeblock_lock();
		status_fix_damage(src, target, damage, ddelay); // We have to seperate here between reflect damage and others [icescope]
		if( attack_type && !status_isdead(target) )
			skill_additional_effect(src, target, skill_id, skill_lv, attack_type, dmg_lv, gettick());
		if( dmg_lv > ATK_BLOCK && attack_type )
			skill_counter_additional_effect(src, target, skill_id, skill_lv, attack_type, gettick());
		map_freeblock_unlock();
		return 0;
	}
	dat = ers_alloc(delay_damage_ers, struct delay_damage);
	dat->src = src;
	dat->target = target->id;
	dat->skill_id = skill_id;
	dat->skill_lv = skill_lv;
	dat->attack_type = attack_type;
	dat->damage = damage;
	dat->dmg_lv = dmg_lv;
	dat->delay = ddelay;
	dat->distance = distance_bl(src, target)+10; //Attack should connect regardless unless you teleported.
	if (src->type != BL_PC && amotion > 1000)
		amotion = 1000; //Aegis places a damage-delay cap of 1 sec to non player attacks. [Skotlex]
	add_timer(tick+amotion, battle_delay_damage_sub, src->id, (intptr)dat);
	
	return 0;
}

int battle_attr_ratio(int atk_elem,int def_type, int def_lv)
{
	
	if (atk_elem < 0 || atk_elem >= ELE_MAX)
		return 100;

	if (def_type < 0 || def_type > ELE_MAX || def_lv < 1 || def_lv > 4)
		return 100;

	return attr_fix_table[def_lv-1][atk_elem][def_type];
}

/*==========================================
 * Does attribute fix modifiers. 
 * Added passing of the chars so that the status changes can affect it. [Skotlex]
 * Note: Passing src/target == NULL is perfectly valid, it skips SC_ checks.
 *------------------------------------------*/
int battle_attr_fix(struct block_list *src, struct block_list *target, int damage,int atk_elem,int def_type, int def_lv)
{
	struct status_change *sc=NULL, *tsc=NULL;
	int ratio;
	
	if (src) sc = status_get_sc(src);
	if (target) tsc = status_get_sc(target);
	
	if (atk_elem < 0 || atk_elem >= ELE_MAX)
		atk_elem = rand()%ELE_MAX;

	if (def_type < 0 || def_type > ELE_MAX ||
		def_lv < 1 || def_lv > 4) {
		ShowError("battle_attr_fix: unknown attr type: atk=%d def_type=%d def_lv=%d\n",atk_elem,def_type,def_lv);
		return damage;
	}

	ratio = attr_fix_table[def_lv-1][atk_elem][def_type];
	if (sc && sc->count)
	{
		if(sc->data[SC_VOLCANO] && atk_elem == ELE_FIRE)
			ratio += enchant_eff[sc->data[SC_VOLCANO]->val1-1];
		if(sc->data[SC_VIOLENTGALE] && atk_elem == ELE_WIND)
			ratio += enchant_eff[sc->data[SC_VIOLENTGALE]->val1-1];
		if(sc->data[SC_DELUGE] && atk_elem == ELE_WATER)
			ratio += enchant_eff[sc->data[SC_DELUGE]->val1-1];
		if(sc->data[SC_FIRE_CLOAK_OPTION] && atk_elem == ELE_FIRE)
			damage += damage * sc->data[SC_FIRE_CLOAK_OPTION]->val2 / 100;
	}
	if( atk_elem == ELE_FIRE && tsc && tsc->count && tsc->data[SC_SPIDERWEB] )
	{
		tsc->data[SC_SPIDERWEB]->val1 = 0; // free to move now
		if( tsc->data[SC_SPIDERWEB]->val2-- > 0 )
			damage <<= 1; // double damage
		if( tsc->data[SC_SPIDERWEB]->val2 == 0 )
			status_change_end(target, SC_SPIDERWEB, INVALID_TIMER);
	}
	if( tsc && tsc->count )
	{
		if(tsc->data[SC_VENOMIMPRESS] && atk_elem == ELE_POISON)
			ratio += tsc->data[SC_VENOMIMPRESS]->val2;
		if(tsc->data[SC_ORATIO] && atk_elem == ELE_HOLY )
			ratio += tsc->data[SC_ORATIO]->val1 * 2;
		if ((tsc->data[SC_FIRE_INSIGNIA] && atk_elem == ELE_WATER) ||
			(tsc->data[SC_WATER_INSIGNIA] && atk_elem == ELE_WIND) ||
			(tsc->data[SC_WIND_INSIGNIA] && atk_elem == ELE_EARTH) ||
			(tsc->data[SC_EARTH_INSIGNIA] && atk_elem == ELE_FIRE))
			ratio += 50;
		if(tsc->data[SC_THORNSTRAP] && atk_elem == ELE_FIRE)
			status_change_end(target, SC_THORNSTRAP, -1);
		if(tsc->data[SC_FIRE_CLOAK_OPTION] && atk_elem == ELE_FIRE)
			damage -= damage * tsc->data[SC_FIRE_CLOAK_OPTION]->val2 / 100;
	}
	if( target && target->type == BL_SKILL )
	{
		if( atk_elem == ELE_FIRE && battle_getcurrentskill(target) == GN_WALLOFTHORN )
		{
			struct skill_unit *su = (struct skill_unit*)target;
			struct skill_unit_group *sg;
			struct block_list *src;
			int x,y;
						
			if( !su || !su->alive || (sg = su->group) == NULL || !sg || sg->val3 == -1 ||
				(src = map_id2bl(su->val2)) == NULL || status_isdead(src) )
				return 0;
			
			if( sg->unit_id != UNT_FIREWALL )
			{
				x = sg->val3 >> 16;
				y = sg->val3 & 0xffff;
				skill_unitsetting(src,su->group->skill_id,su->group->skill_lv,x,y,1);
				sg->val3 = -1;
				sg->limit = DIFF_TICK(gettick(),sg->tick)+300;
			}
		}
	}
	if( atk_elem == ELE_FIRE && tsc && tsc->count && tsc->data[SC_CRYSTALIZE] )
	{ //FIXTHIS: is this right to be an accurate way to remove Crystalize status when hitted by fire element? [Jobbie]
		tsc->data[SC_CRYSTALIZE]->val1 = 0;
		if( tsc->data[SC_CRYSTALIZE]->val2 == 0 )
			status_change_end(target,SC_CRYSTALIZE,-1);
	}
	return damage*ratio/100;
}

/*==========================================
 * �_??[�W??I�v�Z
 *------------------------------------------*/
int battle_calc_damage(struct block_list *src,struct block_list *bl,struct Damage *d,int damage,int skill_num,int skill_lv,int element)
{
	struct map_session_data *sd = NULL;
	struct homun_data *hd = NULL;
	struct status_change *sc, *tsc;
	struct status_change_entry *sce;
	int div_ = d->div_, flag = d->flag;

	nullpo_ret(bl);

	if( !damage )
		return 0;
	if( mob_ksprotected(src, bl) )
		return 0;

	if (bl->type == BL_PC) {
		sd=(struct map_session_data *)bl;
		//Special no damage states
		if(flag&BF_WEAPON && sd->special_state.no_weapon_damage)
			damage -= damage*sd->special_state.no_weapon_damage/100;

		if(flag&BF_MAGIC && sd->special_state.no_magic_damage)
			damage -= damage*sd->special_state.no_magic_damage/100;

		if(flag&BF_MISC && sd->special_state.no_misc_damage)
			damage -= damage*sd->special_state.no_misc_damage/100;

		if(!damage) return 0;
	}
	else if ( bl->type == BL_HOM )
		hd=(struct homun_data *)bl;

	sc = status_get_sc(bl);
	tsc = status_get_sc(src);

	if( sc && sc->data[SC_INVINCIBLE] && !sc->data[SC_INVINCIBLEOFF] )
		return 1;

	if (skill_num == PA_PRESSURE)
		return damage; //This skill bypass everything else.

	if( sc && sc->count )
	{
		//First, sc_*'s that reduce damage to 0.
		if( sc->data[SC_BASILICA] && !(status_get_mode(src)&MD_BOSS) && skill_num != PA_PRESSURE )
		{
			d->dmg_lv = ATK_BLOCK;
			return 0;
		}

		if( sc->data[SC_SAFETYWALL] && (flag&(BF_SHORT|BF_MAGIC)) == BF_SHORT )
		{
			struct skill_unit_group* group = skill_id2group(sc->data[SC_SAFETYWALL]->val3);
			if (group) {
				if (--group->val2<=0)
					skill_delunitgroup(group);
				d->dmg_lv = ATK_BLOCK;
				return 0;
			}
			status_change_end(bl, SC_SAFETYWALL, INVALID_TIMER);
		}

		if( ( (sc->data[SC_PNEUMA] || sc->data[SC_NEUTRALBARRIER] ) && (flag&(BF_MAGIC|BF_LONG)) == BF_LONG) || sc->data[SC__MANHOLE] )
		{
			d->dmg_lv = ATK_BLOCK;
			return 0;
		}		
		if( sc->data[SC_ZEPHYR] && ((flag&BF_LONG) || rand()%100 < 10) )
		{	// TODO: check if it blocks both, magic and physical ranged damages.
			d->dmg_lv = ATK_BLOCK;
			return 0;
		}
		
		if( map_getcell(bl->m,bl->x,bl->y,CELL_CHKMAELSTROM) && (flag&BF_MAGIC) && skill_num && (skill_get_inf(skill_num)&INF_GROUND_SKILL) )
		{
			int sp = damage * 20 / 100; // Steel need official value.
			status_heal(bl,0,sp,3);
			d->dmg_lv = ATK_BLOCK;
			return 0;
		}

		if( sc->data[SC_WEAPONBLOCKING] && flag&(BF_SHORT|BF_WEAPON) && rand()%100 < sc->data[SC_WEAPONBLOCKING]->val2 )
		{
			clif_skill_nodamage(bl,src,GC_WEAPONBLOCKING,1,1);
			d->dmg_lv = ATK_NONE;
			sc_start2(bl,SC_COMBO,100,GC_WEAPONBLOCKING,src->id,2000);
			return 0;
		}

		if( (sce=sc->data[SC_AUTOGUARD]) && flag&BF_WEAPON && !(skill_get_nk(skill_num)&NK_NO_CARDFIX_ATK) && rand()%100 < sce->val2 )
		{
			int delay;
			clif_skill_nodamage(bl,bl,CR_AUTOGUARD,sce->val1,1);
			// different delay depending on skill level [celest]
			if (sce->val1 <= 5)
				delay = 300;
			else if (sce->val1 > 5 && sce->val1 <= 9)
				delay = 200;
			else
				delay = 100;
			unit_set_walkdelay(bl, gettick(), delay, 1);

			if(sc->data[SC_SHRINK] && rand()%100<5*sce->val1)
				skill_blown(bl,src,skill_get_blewcount(CR_SHRINK,1),-1,0);
			return 0;
		}

		if( sd && (sce = sc->data[SC_MILLENNIUMSHIELD]) && sce->val2 > 0 && damage > 0 )
		{
			sce->val3 -= damage; // absorb damage
			d->dmg_lv = ATK_BLOCK;
			if( sce->val3 <= 0 ) // Shield Down
			{
				sce->val2--;
				if( sce->val2 > 0 )
				{
					clif_millenniumshield(sd,sce->val2);
					sce->val3 = 1000; // Next Shield
				}
				else
					status_change_end(bl,SC_MILLENNIUMSHIELD,-1); // All shields down
				status_change_start(bl, SC_STUN, 10000, 0, 0, 0, 0, 1000, 2);
			}
			return 0;
		}

		if( (sce=sc->data[SC_PARRYING]) && flag&BF_WEAPON && skill_num != WS_CARTTERMINATION && rand()%100 < sce->val2 )
		{ // attack blocked by Parrying
			clif_skill_nodamage(bl, bl, LK_PARRYING, sce->val1,1);
			return 0;
		}
		
		if(sc->data[SC_DODGE] && !sc->opt1 &&
			(flag&BF_LONG || sc->data[SC_SPURT])
			&& rand()%100 < 20)
		{
			if (sd && pc_issit(sd) && !sc->data[SC_SITDOWN_FORCE]) pc_setstand(sd); //Stand it to dodge.
			clif_skill_nodamage(bl,bl,TK_DODGE,1,1);
			if (!sc->data[SC_COMBO])
				sc_start4(bl, SC_COMBO, 100, TK_JUMPKICK, src->id, 1, 0, 2000);
			return 0;
		}

		if(sc->data[SC_HERMODE] && flag&BF_MAGIC)
			return 0;

		if(sc->data[SC_TATAMIGAESHI] && (flag&(BF_MAGIC|BF_LONG)) == BF_LONG)
			return 0;

		if((sce=sc->data[SC_KAUPE]) && rand()%100 < sce->val2)
		{	//Kaupe blocks damage (skill or otherwise) from players, mobs, homuns, mercenaries.
			clif_specialeffect(bl, 462, AREA);
			//Shouldn't end until Breaker's non-weapon part connects.
			if (skill_num != ASC_BREAKER || !(flag&BF_WEAPON))
				if (--(sce->val3) <= 0) //We make it work like Safety Wall, even though it only blocks 1 time.
					status_change_end(bl, SC_KAUPE, INVALID_TIMER);
			return 0;
		}

		if( flag&BF_MAGIC && (sce=sc->data[SC_PRESTIGE]) && rand()%100 < sce->val2)
		{
			clif_specialeffect(bl, 462, AREA); // Still need confirm it.
			return 0;
		}

		if (((sce=sc->data[SC_UTSUSEMI]) || sc->data[SC_BUNSINJYUTSU])
		&& 
			flag&BF_WEAPON && !(skill_get_nk(skill_num)&NK_NO_CARDFIX_ATK))
		{
			if (sce) {
				clif_specialeffect(bl, 462, AREA);
				skill_blown(src,bl,sce->val3,-1,0);
			}
			//Both need to be consumed if they are active.
			if (sce && --(sce->val2) <= 0)
				status_change_end(bl, SC_UTSUSEMI, INVALID_TIMER);
			if ((sce=sc->data[SC_BUNSINJYUTSU]) && --(sce->val2) <= 0)
				status_change_end(bl, SC_BUNSINJYUTSU, INVALID_TIMER);
			return 0;
		}

		//Now damage increasing effects
		if( sc->data[SC_AETERNA] && skill_num != PF_SOULBURN )
		{
			if( src->type != BL_MER || skill_num == 0 )
				damage <<= 1; // Lex Aeterna only doubles damage of regular attacks from mercenaries

			if( skill_num != ASC_BREAKER || !(flag&BF_WEAPON) )
				status_change_end(bl, SC_AETERNA, INVALID_TIMER); //Shouldn't end until Breaker's non-weapon part connects.
		}

		if( sc->data[SC_WHITEIMPRISON] )
		{
			if ( element == ELE_GHOST )
			{
				if( skill_num == MG_NAPALMBEAT || skill_num == MG_SOULSTRIKE || skill_num == HW_NAPALMVULCAN || skill_num == WL_SOULEXPANSION )
					damage <<= 1;// Deals double damage with Napalm Beat, Soul Strike, Napalm Vulcan, and Soul Expansion
				status_change_end(bl, SC_WHITEIMPRISON, INVALID_TIMER);
			}
			else
			{
				d->dmg_lv = ATK_BLOCK;
				return 0;
			}
		}

		if( sc->data[SC_DEEPSLEEP] )
		{
			damage += damage / 2; // 1.5 times more damage while in Deep Sleep.
			status_change_end(bl,SC_DEEPSLEEP,-1);
		}
		if( sc->data[SC_VOICEOFSIREN] && damage > 0)
			status_change_end(bl,SC_VOICEOFSIREN,-1);

		if ( sc->data[SC_DARKCROW] && (flag&(BF_SHORT|BF_WEAPON)) == (BF_SHORT|BF_WEAPON))
			damage += damage * sc->data[SC_DARKCROW]->val2 / 100;

		//Finally damage reductions....
		if( sc->data[SC_ASSUMPTIO] )
		{
			if( map_flag_vs(bl->m) )
				damage = damage * 2 / 3; //Receive 66% damage
			else
				damage >>= 1; //Receive 50% damage
		}

		if( sc->data[SC_DEFENDER] && (flag&(BF_LONG|BF_WEAPON)) == (BF_LONG|BF_WEAPON) )
			damage = damage * (100 - sc->data[SC_DEFENDER]->val2) / 100;

		if( sc->data[SC_ADJUSTMENT] && (flag&(BF_LONG|BF_WEAPON)) == (BF_LONG|BF_WEAPON) )
			damage -= 20 * damage / 100;

		if( sc->data[SC_FOGWALL] )
		{
			if( flag&BF_SKILL ) //25% reduction
				damage -= 25 * damage / 100;
			else if( (flag&(BF_LONG|BF_WEAPON)) == (BF_LONG|BF_WEAPON) )
				damage >>= 2; //75% reduction
		}

		if( sc->data[SC_SMOKEPOWDER] )
		{
			if( (flag&(BF_SHORT|BF_WEAPON)) == (BF_SHORT|BF_WEAPON) )
				damage -= 15 * damage / 100;//15% reduction to physical melee attacks
			else if( (flag&(BF_LONG|BF_WEAPON)) == (BF_LONG|BF_WEAPON) )
				damage -= 50 * damage / 100;//50% reduction to physical ranged attacks
		}

		if ( sc->data[SC_SU_STOOP] )
			damage -= damage * 90 / 100;

		// Compressed code, fixed by map.h [Epoque]
		if( src->type == BL_MOB )
		{
			int i;
			if( sc->data[SC_MANU_DEF] )
				for( i = 0; ARRAYLENGTH(mob_manuk) > i; i++ )
					if( mob_manuk[i] == ((TBL_MOB*)src)->class_ )
					{
						damage -= sc->data[SC_MANU_DEF]->val1 * damage / 100;
						break;
					}
			if( sc->data[SC_SPL_DEF] )
				for (i=0;ARRAYLENGTH(mob_splendide)>i;i++)
					if (mob_splendide[i]==((TBL_MOB*)src)->class_) {
						damage -= sc->data[SC_SPL_DEF]->val1*damage/100;
						break;
					}
		}

		if((sce=sc->data[SC_ARMOR]) && //NPC_DEFENDER
			sce->val3&flag && sce->val4&flag)
			damage -= damage*sc->data[SC_ARMOR]->val2/100;

		if(sc->data[SC_ENERGYCOAT] && flag&BF_WEAPON
			&& skill_num != WS_CARTTERMINATION)
		{
			struct status_data *status = status_get_status_data(bl);
			int per = 100*status->sp / status->max_sp -1; //100% should be counted as the 80~99% interval
			per /=20; //Uses 20% SP intervals.
			//SP Cost: 1% + 0.5% per every 20% SP
			if (!status_charge(bl, 0, (10+5*per)*status->max_sp/1000))
				status_change_end(bl, SC_ENERGYCOAT, INVALID_TIMER);
			//Reduction: 6% + 6% every 20%
			damage -= damage * 6 * (1+per) / 100;
		}

		if( (sce = sc->data[SC_STONEHARDSKIN]) && flag&BF_WEAPON && damage > 0 )
		{
			sce->val2 -= damage;
			skill_break_equip(src,EQP_WEAPON,3000,BCT_SELF);// Need to code something that will lower a monster's attack by 25%. [Rytech]

			if( sce->val2 <= 0 ) status_change_end(bl, SC_STONEHARDSKIN, -1);
		}

		// FIXME:
		// So Reject Sword calculates the redirected damage before calculating WoE/BG reduction? This is weird. [Inkfish]
		if((sce=sc->data[SC_REJECTSWORD]) && flag&BF_WEAPON &&
			// Fixed the condition check [Aalye]
			(src->type!=BL_PC || (
				((TBL_PC *)src)->status.weapon == W_DAGGER ||
				((TBL_PC *)src)->status.weapon == W_1HSWORD ||
				((TBL_PC *)src)->status.weapon == W_2HSWORD
			)) &&
			rand()%100 < sce->val2
		){
			damage = damage*50/100;
			status_fix_damage(bl,src,damage,clif_damage(bl,src,gettick(),0,0,damage,0,0,0));
			clif_skill_nodamage(bl,bl,ST_REJECTSWORD,sce->val1,1);
			if(--(sce->val3)<=0)
				status_change_end(bl, SC_REJECTSWORD, INVALID_TIMER);
		}

		//Finally added to remove the status of immobile when aimedbolt is used. [Jobbie]
		if( skill_num == RA_AIMEDBOLT && (sc->data[SC_BITE] || sc->data[SC_ANKLE] || sc->data[SC_ELECTRICSHOCKER]) )
		{
			status_change_end(bl, SC_BITE, -1);
			status_change_end(bl, SC_ANKLE, -1);
			status_change_end(bl, SC_ELECTRICSHOCKER, -1);
		}

		//Finally Kyrie because it may, or not, reduce damage to 0.
		if((sce = sc->data[SC_KYRIE]) && damage > 0){
			sce->val2-=damage;
			if(flag&BF_WEAPON || skill_num == TF_THROWSTONE){
				if(sce->val2>=0)
					damage=0;
				else
				  	damage=-sce->val2;
			}
			if((--sce->val3)<=0 || (sce->val2<=0) || skill_num == AL_HOLYLIGHT)
				status_change_end(bl, SC_KYRIE, INVALID_TIMER);
		}

		if( (sce = sc->data[SC_LIGHTNINGWALK]) && flag&BF_LONG && damage > 0 && rand()%100 < sce->val2 )
		{
			skill_blown(src,bl,distance_bl(src,bl)-1,unit_getdir(src),0);
			d->div_ = ATK_DEF;//The heck is this for? [Rytech]
			status_change_end(bl, SC_LIGHTNINGWALK, -1);
			return 0;
		}

		if (!damage) return 0;

		//Probably not the most correct place, but it'll do here
		//(since battle_drain is strictly for players currently)
		if ((sce=sc->data[SC_BLOODLUST]) && flag&BF_WEAPON && damage > 0 &&
			rand()%100 < sce->val3)
			status_heal(src, damage*sce->val4/100, 0, 3);

		if( sd && (sce = sc->data[SC_FORCEOFVANGUARD]) && flag&BF_WEAPON && rand()%100 < sce->val2 )
			pc_addrageball(sd,skill_get_time(LG_FORCEOFVANGUARD,sce->val1),sce->val3);

		if( sd && (sce = sc->data[SC_GT_ENERGYGAIN]) && flag&BF_WEAPON && rand()%100 < sce->val2 )
		{
			short spheremax = 5;

			if ( sc->data[SC_RAISINGDRAGON] )
				spheremax += sc->data[SC_RAISINGDRAGON]->val1;

			pc_addspiritball(sd, skill_get_time2(SR_GENTLETOUCH_ENERGYGAIN,sce->val1), spheremax);
		}
		
		if( sc->data[SC__DEADLYINFECT] && flag&BF_SHORT && damage > 0 && rand()%100 < 30 + 10 * sc->data[SC__DEADLYINFECT]->val1 )
			status_change_spread(bl, src); // Deadly infect attacked side

		if ( hd && (sce = sc->data[SC_STYLE_CHANGE]) && sce->val1 == GRAPPLER_STYLE && rand()%100 < sce->val2 )
			merc_hom_addspiritball(hd,MAX_HOMUN_SPHERES);

		// Magma Flow autotriggers a splash AoE around self by chance when hit.
		if ( (sce = sc->data[SC_MAGMA_FLOW]) && rand()%100 < 3 * sce->val1)
			skill_castend_nodamage_id(bl,bl,MH_MAGMA_FLOW,sce->val1,0,flag|2);
	}

	if( tsc && tsc->count )
	{
		if( tsc->data[SC_INVINCIBLE] && !tsc->data[SC_INVINCIBLEOFF] )
			damage += damage * 75 / 100;
		// [Epoque]
		if (bl->type == BL_MOB)
		{
			int i;

			if ( ((sce=tsc->data[SC_MANU_ATK]) && (flag&BF_WEAPON)) ||
				 ((sce=tsc->data[SC_MANU_MATK]) && (flag&BF_MAGIC))
				)
				for (i=0;ARRAYLENGTH(mob_manuk)>i;i++)
					if (((TBL_MOB*)bl)->class_==mob_manuk[i]) {
						damage += damage*sce->val1/100;
						break;
					}
			if ( ((sce=tsc->data[SC_SPL_ATK]) && (flag&BF_WEAPON)) ||
				 ((sce=tsc->data[SC_SPL_MATK]) && (flag&BF_MAGIC))
				)
				for (i=0;ARRAYLENGTH(mob_splendide)>i;i++)
					if (((TBL_MOB*)bl)->class_==mob_splendide[i]) {
						damage += damage*sce->val1/100;
						break;
					}
		}

		if( tsc->data[SC_POISONINGWEAPON] )
		{
			struct status_data *tstatus;
			short rate = 100;
			tstatus = status_get_status_data(bl);
			if ( !(flag&BF_SKILL) && (flag&BF_WEAPON) && damage > 0 && rand()%100 < tsc->data[SC_POISONINGWEAPON]->val3 )
			{
				if ( tsc->data[SC_POISONINGWEAPON]->val1 == 9 )//Oblivion Curse gives a 2nd success chance after the 1st one passes which is reduceable. [Rytech]
					rate = 100 - tstatus->int_ * 4 / 5 ;
				sc_start(bl,tsc->data[SC_POISONINGWEAPON]->val2,rate,tsc->data[SC_POISONINGWEAPON]->val1,skill_get_time2(GC_POISONINGWEAPON,1) - (tstatus->vit + tstatus->luk) / 2 * 1000);
			}
		}

		if( tsc->data[SC__DEADLYINFECT] && flag&BF_SHORT && damage > 0 && rand()%100 < 30 + 10 * tsc->data[SC__DEADLYINFECT]->val1 )
			status_change_spread(src, bl);
	}

	if( battle_config.pk_mode && sd && bl->type == BL_PC && damage )
  	{
		if( flag & BF_SKILL )
		{ //Skills get a different reduction than non-skills. [Skotlex]
			if( flag&BF_WEAPON )
				damage = damage * battle_config.pk_weapon_damage_rate/100;
			if( flag&BF_MAGIC )
				damage = damage * battle_config.pk_magic_damage_rate/100;
			if( flag&BF_MISC )
				damage = damage * battle_config.pk_misc_damage_rate/100;
		}
		else
		{ //Normal attacks get reductions based on range.
			if( flag&BF_SHORT )
				damage = damage * battle_config.pk_short_damage_rate/100;
			if( flag&BF_LONG )
				damage = damage * battle_config.pk_long_damage_rate/100;
		}
		if( !damage ) damage  = 1;
	}

	if(battle_config.skill_min_damage && damage > 0 && damage < div_)
	{
		if ((flag&BF_WEAPON && battle_config.skill_min_damage&1)
			|| (flag&BF_MAGIC && battle_config.skill_min_damage&2)
			|| (flag&BF_MISC && battle_config.skill_min_damage&4)
		)
			damage = div_;
	}

	if( bl->type == BL_MOB && !status_isdead(bl) && src != bl) {
	  if (damage > 0 )
			mobskill_event((TBL_MOB*)bl,src,gettick(),flag);
	  if (skill_num)
			mobskill_event((TBL_MOB*)bl,src,gettick(),MSC_SKILLUSED|(skill_num<<16));
	}

	if( sd && pc_ismadogear(sd) && (element == ELE_FIRE || element == ELE_WATER) && rand()%100 < 50 )
		pc_overheat(sd,element == ELE_FIRE ? 1 : -1);
		
	if( sc && sc->data[SC__SHADOWFORM]  )
	{
		struct block_list *s_bl = map_id2bl(sc->data[SC__SHADOWFORM]->val2);
		if( !s_bl )
		{ // If the shadow form target is not present remove the sc.
			status_change_end(bl, SC__SHADOWFORM, -1);
		}
		else if( status_isdead(s_bl) || !battle_check_target(src,s_bl,BCT_ENEMY))
		{ // If the shadow form target is dead or not your enemy remove the sc in both.
			status_change_end(bl, SC__SHADOWFORM, -1);
			if( s_bl->type == BL_PC )
				((TBL_PC*)s_bl)->shadowform_id = 0;
		}
		else
		{
			if( (--sc->data[SC__SHADOWFORM]->val3) < 0 )
			{ // If you have exceded max hits supported, remove the sc in both.
				status_change_end(bl, SC__SHADOWFORM, -1);
				if( s_bl->type == BL_PC )
					((TBL_PC*)s_bl)->shadowform_id = 0;
			}
			else
			{
				status_damage(src, s_bl, damage, 0, clif_damage(s_bl, s_bl, gettick(), 500, 500, damage, -1, 0, 0), 0);
				return ATK_NONE;
			}
		}
	}

	return damage;
}

/*==========================================
 * Calculates BG related damage adjustments.
 *------------------------------------------*/
int battle_calc_bg_damage(struct block_list *src, struct block_list *bl, int damage, int div_, int skill_num, int skill_lv, int flag)
{
	if( !damage )
		return 0;

	if( bl->type == BL_MOB )
	{
		struct mob_data* md = BL_CAST(BL_MOB, bl);
		if( map[bl->m].flag.battleground && (md->class_ == 1914 || md->class_ == 1915) && flag&BF_SKILL )
			return 0; // Crystal cannot receive skill damage on battlegrounds
	}

	switch( skill_num )
	{
		case PA_PRESSURE:
		case HW_GRAVITATION:
		case NJ_ZENYNAGE:
		//case RK_DRAGONBREATH:
		//case GN_HELLS_PLANT_ATK:
		//case KO_MUCHANAGE:
			break;
		default:
			if( flag&BF_SKILL )
			{ //Skills get a different reduction than non-skills. [Skotlex]
				if( flag&BF_WEAPON )
					damage = damage * battle_config.bg_weapon_damage_rate/100;
				if( flag&BF_MAGIC )
					damage = damage * battle_config.bg_magic_damage_rate/100;
				if(	flag&BF_MISC )
					damage = damage * battle_config.bg_misc_damage_rate/100;
			}
			else
			{ //Normal attacks get reductions based on range.
				if( flag&BF_SHORT )
					damage = damage * battle_config.bg_short_damage_rate/100;
				if( flag&BF_LONG )
					damage = damage * battle_config.bg_long_damage_rate/100;
			}
			
			if( !damage ) damage = 1;
	}

	return damage;
}

/*==========================================
 * Calculates GVG related damage adjustments.
 *------------------------------------------*/
int battle_calc_gvg_damage(struct block_list *src,struct block_list *bl,int damage,int div_,int skill_num,int skill_lv,int flag)
{
	struct mob_data* md = BL_CAST(BL_MOB, bl);
	int class_ = status_get_class(bl);

	if (!damage) //No reductions to make.
		return 0;
	
	if(md && md->guardian_data) {
		if(class_ == MOBID_EMPERIUM && flag&BF_SKILL)
		//Skill immunity.
			switch (skill_num) {
			case MO_TRIPLEATTACK:
			case HW_GRAVITATION:
				break;
			default:
				return 0;
		}
		if(src->type != BL_MOB) {
			struct guild *g=guild_search(status_get_guild_id(src));
			if (!g) return 0;
			if (class_ == MOBID_EMPERIUM && guild_checkskill(g,GD_APPROVAL) <= 0)
				return 0;
			if (battle_config.guild_max_castles && guild_checkcastles(g)>=battle_config.guild_max_castles)
				return 0; // [MouseJstr]
		}
	}

	switch (skill_num) {
	//Skills with no damage reduction.
	case PA_PRESSURE:
	case HW_GRAVITATION:
	case NJ_ZENYNAGE:
	//case RK_DRAGONBREATH:
	//case GN_HELLS_PLANT_ATK:
	//case KO_MUCHANAGE:
		break;
	default:
		/* Uncomment if you want god-mode Emperiums at 100 defense. [Kisuka]
		if (md && md->guardian_data) {
			damage -= damage * (md->guardian_data->castle->defense/100) * battle_config.castle_defense_rate/100;
		}
		*/
		if (flag & BF_SKILL) { //Skills get a different reduction than non-skills. [Skotlex]
			if (flag&BF_WEAPON)
				damage = damage * battle_config.gvg_weapon_damage_rate/100;
			if (flag&BF_MAGIC)
				damage = damage * battle_config.gvg_magic_damage_rate/100;
			if (flag&BF_MISC)
				damage = damage * battle_config.gvg_misc_damage_rate/100;
		} else { //Normal attacks get reductions based on range.
			if (flag & BF_SHORT)
				damage = damage * battle_config.gvg_short_damage_rate/100;
			if (flag & BF_LONG)
				damage = damage * battle_config.gvg_long_damage_rate/100;
		}
		if(!damage) damage  = 1;
	}
	return damage;
}

/*==========================================
 * HP/SP�z���̌v�Z
 *------------------------------------------*/
static int battle_calc_drain(int damage, int rate, int per)
{
	int diff = 0;

	if (per && rand()%1000 < rate) {
		diff = (damage * per) / 100;
		if (diff == 0) {
			if (per > 0)
				diff = 1;
			else
				diff = -1;
		}
	}
	return diff;
}

/*==========================================
 * ?C���_??[�W
 *------------------------------------------*/
int battle_addmastery(struct map_session_data *sd,struct block_list *target,int dmg,int type)
{
	int damage,skill;
	struct status_data *status = status_get_status_data(target);
	int weapon;
	damage = dmg;

	nullpo_ret(sd);

	if((skill = pc_checkskill(sd,AL_DEMONBANE)) > 0 &&
		target->type == BL_MOB && //This bonus doesnt work against players.
		(battle_check_undead(status->race,status->def_ele) || status->race==RC_DEMON) )
		damage += (skill*(int)(3+(sd->status.base_level+1)*0.05));	// submitted by orn
		//damage += (skill * 3);

	if((skill = pc_checkskill(sd,HT_BEASTBANE)) > 0 && (status->race==RC_BRUTE || status->race==RC_INSECT) ) {
		damage += (skill * 4);
		if (sd->sc.data[SC_SPIRIT] && sd->sc.data[SC_SPIRIT]->val2 == SL_HUNTER)
			damage += sd->status.str;
	}

	if( (skill = pc_checkskill(sd, RA_RANGERMAIN)) > 0 && (status->race == RC_BRUTE || status->race == RC_PLANT || status->race == RC_FISH) )
		damage += (skill * 5);

	if( (skill = pc_checkskill(sd,NC_RESEARCHFE)) > 0 && (status->def_ele == ELE_FIRE || status->def_ele == ELE_EARTH) )
		damage += (skill * 10);
	
	if( (skill = pc_checkskill(sd,NC_MADOLICENCE)) > 0 && pc_ismadogear(sd) )
		damage += (skill * 15);

	if(type == 0)
		weapon = sd->weapontype1;
	else
		weapon = sd->weapontype2;
	switch(weapon)
	{
		case W_DAGGER:
		case W_1HSWORD:
			if((skill = pc_checkskill(sd,SM_SWORD)) > 0)
				damage += (skill * 4);
			if((skill = pc_checkskill(sd,GN_TRAINING_SWORD)) > 0)
				damage += skill * 10;
			break;
		case W_2HSWORD:
			if((skill = pc_checkskill(sd,SM_TWOHAND)) > 0)
				damage += (skill * 4);
			break;
		case W_1HSPEAR:
		case W_2HSPEAR:
			if((skill = pc_checkskill(sd,KN_SPEARMASTERY)) > 0) {
				if(pc_isdragon(sd))
					damage += (skill * 10);
				else if(pc_isriding(sd))
					damage += (skill * 5);
				else
					damage += (skill * 4);
			}
			break;
		case W_1HAXE:
		case W_2HAXE:
			if((skill = pc_checkskill(sd,AM_AXEMASTERY)) > 0)
				damage += (skill * 3);
			if((skill = pc_checkskill(sd,NC_TRAININGAXE)) > 0)
				damage += (skill * 5);
			break;
		case W_MACE:
		case W_2HMACE:
			if((skill = pc_checkskill(sd,PR_MACEMASTERY)) > 0)
				damage += (skill * 3);
			if((skill = pc_checkskill(sd,NC_TRAININGAXE)) > 0)
				damage += (skill * 4);
			break;
		case W_FIST:
			if((skill = pc_checkskill(sd,TK_RUN)) > 0)
				damage += (skill * 10);
			// No break, fallthrough to Knuckles
		case W_KNUCKLE:
			if((skill = pc_checkskill(sd,MO_IRONHAND)) > 0)
				damage += (skill * 3);
			break;
		case W_MUSICAL:
			if((skill = pc_checkskill(sd,BA_MUSICALLESSON)) > 0)
				damage += (skill * 3);
			break;
		case W_WHIP:
			if((skill = pc_checkskill(sd,DC_DANCINGLESSON)) > 0)
				damage += (skill * 3);
			break;
		case W_BOOK:
			if((skill = pc_checkskill(sd,SA_ADVANCEDBOOK)) > 0)
				damage += (skill * 3);
			break;
		case W_KATAR:
			if((skill = pc_checkskill(sd,AS_KATAR)) > 0)
				damage += (skill * 3);
			break;
	}

	return damage;
}
/*==========================================
 * Calculates the standard damage of a normal attack assuming it hits,
 * it calculates nothing extra fancy, is needed for magnum break's WATK_ELEMENT bonus. [Skotlex]
 *------------------------------------------
 * Pass damage2 as NULL to not calc it.
 * Flag values:
 * &1: Critical hit
 * &2: Arrow attack
 * &4: Skill is Magic Crasher
 * &8: Skip target size adjustment (Extremity Fist?)
 *&16: Arrow attack but BOW, REVOLVER, RIFLE, SHOTGUN, GATLING or GRENADE type weapon not equipped (i.e. shuriken, kunai and venom knives not affected by DEX)
 */
static int battle_calc_base_damage(struct status_data *status, struct weapon_atk *wa, struct status_change *sc, unsigned short t_size, struct map_session_data *sd, int flag)
{
	unsigned short atkmin = 0, atkmax = 0;
	short type = 0;
	int damage = 0;

	if( !sd )
	{	//Mobs/Pets
		if( flag&4 )
		{
			atkmin = status->matk_min;
			atkmax = status->matk_max;
		} else {
			atkmin = wa->atk;
			atkmax = wa->atk2;
		}
		if (atkmin > atkmax)
			atkmin = atkmax;
	} else {	//PCs
		atkmax = wa->atk;
		type = (wa == &status->lhw)?EQI_HAND_L:EQI_HAND_R;

		if (!(flag&1) || (flag&2))
		{	//Normal attacks
			atkmin = status->dex;
			
			if (sd->equip_index[type] >= 0 && sd->inventory_data[sd->equip_index[type]])
				atkmin = atkmin*(80 + sd->inventory_data[sd->equip_index[type]]->wlv*20)/100;

			if (atkmin > atkmax)
				atkmin = atkmax;
			
			if(flag&2 && !(flag&16))
			{	//Bows
				atkmin = atkmin*atkmax/100;
				if (atkmin > atkmax)
					atkmax = atkmin;
			}
		}
	}
	
	if (sc && sc->data[SC_MAXIMIZEPOWER])
		atkmin = atkmax;
	
	//Weapon Damage calculation
	if (!(flag&1))
		damage = (atkmax>atkmin? rand()%(atkmax-atkmin):0)+atkmin;
	else 
		damage = atkmax;
	
	if (sd)
	{
		//rodatazone says the range is 0~arrow_atk-1 for non crit
		if (flag&2 && sd->arrow_atk)
			damage += ((flag&1)?sd->arrow_atk:rand()%sd->arrow_atk);

		//SizeFix only for players
		if (!(sd->special_state.no_sizefix || (flag&8)))
			damage = damage*(type==EQI_HAND_L?
				sd->left_weapon.atkmods[t_size]:
				sd->right_weapon.atkmods[t_size])/100;
	}
	
	//Finally, add baseatk
	if(flag&4)
		damage += status->matk_min;
	else
		damage += status->batk;
	
	//rodatazone says that Overrefine bonuses are part of baseatk
	//Here we also apply the weapon_atk_rate bonus so it is correctly applied on left/right hands.
	if(sd) {
		if (type == EQI_HAND_L) {
			if(sd->left_weapon.overrefine)
				damage += rand()%sd->left_weapon.overrefine+1;
			if (sd->weapon_atk_rate[sd->weapontype2])
				damage += damage*sd->weapon_atk_rate[sd->weapontype2]/100;;
		} else { //Right hand
			if(sd->right_weapon.overrefine)
				damage += rand()%sd->right_weapon.overrefine+1;
			if (sd->weapon_atk_rate[sd->weapontype1])
				damage += damage*sd->weapon_atk_rate[sd->weapontype1]/100;;
		}
	}
	return damage;
}

/*==========================================
 * Consumes ammo for the given skill.
 *------------------------------------------*/
void battle_consume_ammo(TBL_PC*sd, int skill, int lv)
{
	int qty=1;
	if (!battle_config.arrow_decrement)
		return;
	
	if (skill)
	{
		qty = skill_get_ammo_qty(skill, lv);
		if (!qty) qty = 1;
	}

	if(sd->equip_index[EQI_AMMO]>=0) //Qty check should have been done in skill_check_condition
		pc_delitem(sd,sd->equip_index[EQI_AMMO],qty,0,1);

	sd->state.arrow_atk = 0;
}

static int battle_range_type(
	struct block_list *src, struct block_list *target,
	int skill_num, int skill_lv)
{	//Skill Range Criteria
	if (battle_config.skillrange_by_distance &&
		(src->type&battle_config.skillrange_by_distance)
	) { //based on distance between src/target [Skotlex]
		if (check_distance_bl(src, target, 5))
			return BF_SHORT;
		return BF_LONG;
	}
	//based on used skill's range
	if (skill_get_range2(src, skill_num, skill_lv) < 5)
		return BF_SHORT;
	return BF_LONG;
}

static int battle_blewcount_bonus(struct map_session_data *sd, int skill_num)
{
	int i;
	if (!sd->skillblown[0].id)
		return 0;
	//Apply the bonus blewcount. [Skotlex]
	for (i = 0; i < ARRAYLENGTH(sd->skillblown) && sd->skillblown[i].id; i++) {
		if (sd->skillblown[i].id == skill_num)
			return sd->skillblown[i].val;
	}
	return 0;
}

struct Damage battle_calc_magic_attack(struct block_list *src,struct block_list *target,int skill_num,int skill_lv,int mflag);
struct Damage battle_calc_misc_attack(struct block_list *src,struct block_list *target,int skill_num,int skill_lv,int mflag);

//For quick div adjustment.
#define damage_div_fix(dmg, div) { if (div > 1) (dmg)*=div; else if (div < 0) (div)*=-1; }
/*==========================================
 * battle_calc_weapon_attack (by Skotlex)
 *------------------------------------------*/
static struct Damage battle_calc_weapon_attack(struct block_list *src, struct block_list *target, int skill_num, int skill_lv, int wflag)
{
	unsigned int skillratio = 100;	//Skill dmg modifiers.
	short skill=0;
	short s_ele, s_ele_, t_class;
	int i, nk, s_level;
	bool n_ele = false; // non-elemental
	char re_baselv_bonus = battle_config.renewal_baselvl_skill_ratio;//BaseLv config for renewal skills.
	int chorusbonus = 0;//Chorus bonus value for chorus skills. Bonus remains 0 unless 3 or more Minstrel's/Wanderer's are in the party.

	struct map_session_data *sd, *tsd;
	struct homun_data *hd;
	struct Damage wd;
	struct status_change *sc = status_get_sc(src);
	struct status_change *tsc = status_get_sc(target);
	struct status_data *sstatus = status_get_status_data(src);
	struct status_data *tstatus = status_get_status_data(target);
	struct {
		unsigned hit : 1; //the attack Hit? (not a miss)
		unsigned cri : 1;		//Critical hit
		unsigned idef : 1;	//Ignore defense
		unsigned idef2 : 1;	//Ignore defense (left weapon)
		unsigned pdef : 2;	//Pierces defense (Investigate/Ice Pick)
		unsigned pdef2 : 2;	//1: Use def+def2/100, 2: Use def+def2/50	
		unsigned infdef : 1;	//Infinite defense (plants)
		unsigned arrow : 1;	//Attack is arrow-based
		unsigned rh : 1;		//Attack considers right hand (wd.damage)
		unsigned lh : 1;		//Attack considers left hand (wd.damage2)
		unsigned weapon : 1; //It's a weapon attack (consider VVS, and all that)
	}	flag;	

	memset(&wd,0,sizeof(wd));
	memset(&flag,0,sizeof(flag));

	if(src==NULL || target==NULL)
	{
		nullpo_info(NLP_MARK);
		return wd;
	}

	s_level = status_get_lv(src);
	if( skill_num >= RK_ENCHANTBLADE && skill_num <= LG_OVERBRAND_PLUSATK &&
		battle_config.max_highlvl_nerf && s_level > battle_config.max_highlvl_nerf )
		s_level = battle_config.max_highlvl_nerf;

	//Initial flag
	flag.rh = 1;
	flag.weapon = 1;
	flag.infdef =(tstatus->mode&MD_PLANT) ? 1 : 0;

	if( target->type == BL_SKILL && ((TBL_SKILL*)target)->group->unit_id == UNT_REVERBERATION )
		flag.infdef = 1; // Reberberation takes 1 damage

	//Initial Values
	wd.type = 0; //Normal attack
	wd.div_ = skill_num?skill_get_num(skill_num,skill_lv):1;
	wd.amotion = (skill_num && skill_get_inf(skill_num)&INF_GROUND_SKILL)?0:sstatus->amotion; //Amotion should be 0 for ground skills.
	if( skill_num == KN_AUTOCOUNTER )
		wd.amotion >>= 1;
	wd.dmotion = tstatus->dmotion;
	wd.blewcount = skill_get_blewcount(skill_num,skill_lv);
	wd.flag = BF_WEAPON; //Initial Flag
	wd.flag |= (skill_num||wflag)?BF_SKILL:BF_NORMAL; // Baphomet card's splash damage is counted as a skill. [Inkfish]
	wd.dmg_lv = ATK_DEF;	//This assumption simplifies the assignation later
	nk = skill_get_nk(skill_num);
	if( !skill_num && wflag ) //If flag, this is splash damage from Baphomet Card and it always hits.
		nk |= NK_NO_CARDFIX_ATK|NK_IGNORE_FLEE;
	flag.hit = nk&NK_IGNORE_FLEE?1:0;
	flag.idef = flag.idef2 = nk&NK_IGNORE_DEF?1:0;

	if (sc && !sc->count)
		sc = NULL; //Skip checking as there are no status changes active.
	if (tsc && !tsc->count)
		tsc = NULL; //Skip checking as there are no status changes active.

	sd = BL_CAST(BL_PC, src);
	tsd = BL_CAST(BL_PC, target);
	hd = BL_CAST(BL_HOM, src);

	// Minstrel/Wanderer number check for chorus skills.
	// Bonus remains 0 unless 3 or more Minstrel's/Wanderer's are in the party.
	if( sd && sd->status.party_id && party_foreachsamemap(party_sub_count_chorus, sd, 0) > 7)
		chorusbonus = 5;//Maximum effect possiable from 7 or more Minstrel's/Wanderer's
	else if( sd && sd->status.party_id && party_foreachsamemap(party_sub_count_chorus, sd, 0) > 2)
		chorusbonus = party_foreachsamemap(party_sub_count_chorus, sd, 0) - 2;//Effect bonus from additional Minstrel's/Wanderer's if not above the max possiable.

	if(sd)
		wd.blewcount += battle_blewcount_bonus(sd, skill_num);

	//Set miscellaneous data that needs be filled regardless of hit/miss
	if(
		(sd && sd->state.arrow_atk) ||
		(!sd && ((skill_num && skill_get_ammotype(skill_num)) || sstatus->rhw.range>3))
	)
		flag.arrow = 1;
	
	if(skill_num){
		wd.flag |= battle_range_type(src, target, skill_num, skill_lv);
		switch(skill_num)
		{
			case MO_FINGEROFFENSIVE:
				if(sd) {
					if (battle_config.finger_offensive_type)
						wd.div_ = 1;
					else
						wd.div_ = sd->spiritball_old;
				}
				break;
			case HT_PHANTASMIC:
				//Since these do not consume ammo, they need to be explicitly set as arrow attacks.
				flag.arrow = 1;
				break;

			case CR_SHIELDBOOMERANG:
			case PA_SHIELDCHAIN:
			case LG_SHIELDPRESS:
			case LG_EARTHDRIVE:
			case RK_DRAGONBREATH:
			case NC_SELFDESTRUCTION:
			case RK_DRAGONBREATH_WATER:
				flag.weapon = 0;
				break;

			case KN_PIERCE:
			case ML_PIERCE:
				wd.div_= (wd.div_>0?tstatus->size+1:-(tstatus->size+1));
				break;

			case TF_DOUBLE: //For NPC used skill.
			case GS_CHAINACTION:
				wd.type = 0x08;
				break;
				
			case GS_GROUNDDRIFT:
			case KN_SPEARSTAB:
			case KN_BOWLINGBASH:
			case MS_BOWLINGBASH:
			case MO_BALKYOUNG:
			case TK_TURNKICK:
				wd.blewcount=0;
				break;

			case RL_R_TRIP:// Knock's back target out of skill range.
				wd.blewcount = wd.blewcount - distance_bl(src,target);
				break;

			case KN_AUTOCOUNTER:
				wd.flag=(wd.flag&~BF_SKILLMASK)|BF_NORMAL;
				break;

			case NPC_CRITICALSLASH:
			case LG_PINPOINTATTACK:
				flag.cri = 1; //Always critical skill.
				break;

			case LK_SPIRALPIERCE:
				if (!sd) wd.flag=(wd.flag&~(BF_RANGEMASK|BF_WEAPONMASK))|BF_LONG|BF_MISC;
				break;

			//The number of hits is set to 3 by default for use in Inspiration status.
			//When in banding, the number of hits is equal to the number of Royal Guards in banding.
			case LG_HESPERUSLIT:
				if( sc && sc->data[SC_BANDING] && sc->data[SC_BANDING]->val2 > 3 )
					wd.div_ = sc->data[SC_BANDING]->val2;
				break;

			case MH_SONIC_CRAW:
				if (hd)
					wd.div_ = hd->hom_spiritball_old;
				else
					wd.div_ = 10;
				break;

			case EL_STONE_RAIN:
				if( !(wflag&1) )
					wd.div_ = 1;
				break;
		}
	} else //Range for normal attacks.
		wd.flag |= flag.arrow?BF_LONG:BF_SHORT;
	
	if ( (!skill_num || skill_num == PA_SACRIFICE) && tstatus->flee2 && rand()%1000 < tstatus->flee2 )
	{	//Check for Lucky Dodge
		wd.type=0x0b;
		wd.dmg_lv=ATK_LUCKY;
		if (wd.div_ < 0) wd.div_*=-1;
		return wd;
	}

	t_class = status_get_class(target);
	s_ele = s_ele_ = skill_get_ele(skill_num, skill_lv);
	if( !skill_num || s_ele == -1 )
	{ //Take weapon's element
		s_ele = sstatus->rhw.ele;
		s_ele_ = sstatus->lhw.ele;
		if( flag.arrow && sd && sd->arrow_ele )
			s_ele = sd->arrow_ele;
		if( battle_config.attack_attr_none&src->type )
			n_ele = true; //Weapon's element is "not elemental"
	}
	else if( s_ele == -2 ) //Use enchantment's element
		s_ele = s_ele_ = status_get_attack_sc_element(src,sc);
	else if( s_ele == -3 ) //Use random element
		s_ele = s_ele_ = rand()%ELE_MAX;
	switch( skill_num )
	{
		case GS_GROUNDDRIFT:
			s_ele = s_ele_ = wflag; //element comes in flag.
			break;
		case LK_SPIRALPIERCE:
			if (!sd) n_ele = false; //forced neutral for monsters
			break;
	}

	if(!skill_num)
  	{	//Skills ALWAYS use ONLY your right-hand weapon (tested on Aegis 10.2)
		if (sd && sd->weapontype1 == 0 && sd->weapontype2 > 0)
		{
			flag.rh=0;
			flag.lh=1;
		}
		if (sstatus->lhw.atk)
			flag.lh=1;
	}

	if( sd && !skill_num )
	{
		short dachance = 0;//Success chance of double attacking. If player is in fear breeze status and generated number is within fear breeze's range, this will be ignored.
		short hitnumber = 0;//Used for setting how many hits will hit.
		short gendetect[] = { 12, 12, 21, 27, 30 };//If generated number is outside this value while in fear breeze status, it will check if their's a chance for double attacking.
		short generate = rand()%100 + 1;//Generates a random number between 1 - 100 which is then used to determine if fear breeze or double attacking will happen.

		// First we go through a number of checks to see if their's any chance of double attacking a target. Only the highest success chance is taken.

		// Double Attack chance from cards and equips.
		if ( sd->double_rate > 0 && sd->weapontype1 != W_FIST )
			dachance = sd->double_rate;

		// Shadow Warrior - Buff Skill
		if ( sc && sc->data[SC_KAGEMUSYA] && 10 * sc->data[SC_KAGEMUSYA]->val1 > dachance && sd->weapontype1 != W_FIST )
			dachance = 10 * sc->data[SC_KAGEMUSYA]->val1;

		// Double Attack - Passive Skill
		if ( 5 * pc_checkskill(sd,TF_DOUBLE) > dachance && sd->weapontype1 == W_DAGGER )
			dachance = 5 * pc_checkskill(sd,TF_DOUBLE);

		// Eternal Chain - Buff Skill
		if ( sc && sc->data[SC_E_CHAIN] && 5 * sc->data[SC_E_CHAIN]->val1 > dachance && sd->weapontype1 != W_FIST )
			dachance = 5 * sc->data[SC_E_CHAIN]->val1;

		// Chain Action - Passive Skill
		if ( 5 * pc_checkskill(sd,GS_CHAINACTION) > dachance && sd->weapontype1 == W_REVOLVER )
			dachance = 5 * pc_checkskill(sd,GS_CHAINACTION);

		// Fear Breeze - Buff Skill
		// This checks if the generated value is within fear breeze's success chance range for the level used as set by gendetect.
		if ( sc && sc->data[SC_FEARBREEZE] && generate <= gendetect[sc->data[SC_FEARBREEZE]->val1 - 1] && sd->weapontype1 == W_BOW )
		{
				if ( generate >= 1 && generate <= 12 )//12% chance to deal 2 hits.
					hitnumber = 2;
				else if ( generate >= 13 && generate <= 21 )//9% chance to deal 3 hits.
					hitnumber = 3;
				else if ( generate >= 22 && generate <= 27 )//6% chance to deal 4 hits.
					hitnumber = 4;
				else if ( generate >= 28 && generate <= 30 )//3% chance to deal 5 hits.
					hitnumber = 5;
		}
		// If the generated value is higher then Fear Breeze's success chance range, but not higher then the player's double attack success chance,
		// then allow a double attack to happen.
		else if ( generate < dachance )
			hitnumber = 2;

		if ( hitnumber > 1 )//Needed to allow critical attacks to hit when not hitting more then once.
			{wd.div_ = hitnumber;
			wd.type = 0x08;}
	}

	//Check for critical
	if( !flag.cri && !(wd.type&0x08) && sstatus->cri &&
		(!skill_num ||
		skill_num == KN_AUTOCOUNTER ||
		skill_num == SN_SHARPSHOOTING || skill_num == MA_SHARPSHOOTING ||
		skill_num == NJ_KIRIKAGE))
	{
		short cri = sstatus->cri;
		if (sd)
		{
			cri+= sd->critaddrace[tstatus->race];
			if(flag.arrow)
				cri += sd->arrow_cri;
			if(sd->status.weapon == W_KATAR)
				cri <<=1;
		}
		//The official equation is *2, but that only applies when sd's do critical.
		//Therefore, we use the old value 3 on cases when an sd gets attacked by a mob
		cri -= tstatus->luk*(!sd&&tsd?3:2);
		
		if(tsc)
		{
			if (tsc->data[SC_SLEEP])
				cri <<=1;
		}
		switch (skill_num)
		{
			case KN_AUTOCOUNTER:
				if(battle_config.auto_counter_type &&
					(battle_config.auto_counter_type&src->type))
					flag.cri = 1;
				else
					cri <<= 1;
				break;
			case SN_SHARPSHOOTING:
			case MA_SHARPSHOOTING:
				cri += 200;
				break;
			case NJ_KIRIKAGE:
				cri += 250 + 50*skill_lv;
				break;
		}
		if(tsd && tsd->critical_def)
			cri = cri*(100-tsd->critical_def)/100;
		if (rand()%1000 < cri) // Steel need confirm if critical def reduce it.
			flag.cri= 1;
	}
	if (flag.cri)
	{
		wd.type = 0x0a;
		flag.idef = flag.idef2 = flag.hit = 1;
	} else {	//Check for Perfect Hit
		if(sd && sd->perfect_hit > 0 && rand()%100 < sd->perfect_hit)
			flag.hit = 1;
		if (sc && sc->data[SC_FUSION]) {
			flag.hit = 1; //SG_FUSION always hit [Komurka]
			flag.idef = flag.idef2 = 1; //def ignore [Komurka]
		}
		if( !flag.hit )
			switch(skill_num)
			{
				case AS_SPLASHER:
					if( !wflag ) // Always hits the one exploding.
						flag.hit = 1;
					break;
				case CR_SHIELDBOOMERANG:
					if( sc && sc->data[SC_SPIRIT] && sc->data[SC_SPIRIT]->val2 == SL_CRUSADER )
						flag.hit = 1;
					break;
			}
		if (tsc && !flag.hit && tsc->opt1 && tsc->opt1 != OPT1_STONEWAIT)
			flag.hit = 1;
	}

	if (!flag.hit)
	{	//Hit/Flee calculation
		short
			flee = tstatus->flee,
			hitrate=80; //Default hitrate

		if(battle_config.agi_penalty_type &&
			battle_config.agi_penalty_target&target->type)
		{	
			unsigned char attacker_count; //256 max targets should be a sane max
			attacker_count = unit_counttargeted(target,battle_config.agi_penalty_count_lv);
			if(attacker_count >= battle_config.agi_penalty_count)
			{
				if (battle_config.agi_penalty_type == 1)
					flee = (flee * (100 - (attacker_count - (battle_config.agi_penalty_count - 1))*battle_config.agi_penalty_num))/100;
				else //asume type 2: absolute reduction
					flee -= (attacker_count - (battle_config.agi_penalty_count - 1))*battle_config.agi_penalty_num;
				if(flee < 1) flee = 1;
			}
		}

		hitrate+= sstatus->hit - flee;

		if(wd.flag&BF_LONG && !skill_num && //Fogwall's hit penalty is only for normal ranged attacks.
			tsc && tsc->data[SC_FOGWALL])
			hitrate -= 50;

		if(sd && flag.arrow)
			hitrate += sd->arrow_hit;
		if(skill_num)
			switch(skill_num)
		{	//Hit skill modifiers
			//It is proven that bonus is applied on final hitrate, not hit.
			case SM_BASH:
			case MS_BASH:
				hitrate += hitrate * 5 * skill_lv / 100;
				break;
			case MS_MAGNUM:
			case SM_MAGNUM:
				hitrate += hitrate * 10 * skill_lv / 100;
				break;
			case KN_AUTOCOUNTER:
			case PA_SHIELDCHAIN:
			case NPC_WATERATTACK:
			case NPC_GROUNDATTACK:
			case NPC_FIREATTACK:
			case NPC_WINDATTACK:
			case NPC_POISONATTACK:
			case NPC_HOLYATTACK:
			case NPC_DARKNESSATTACK:
			case NPC_UNDEADATTACK:
			case NPC_TELEKINESISATTACK:
			case NPC_BLEEDING:
				hitrate += hitrate * 20 / 100;
				break;
			case KN_PIERCE:
			case ML_PIERCE:
				hitrate += hitrate * 5 * skill_lv / 100;
				break;
			case AS_SONICBLOW:
				if(sd && pc_checkskill(sd,AS_SONICACCEL)>0)
					hitrate += hitrate * 50 / 100;
				break;
			case MC_CARTREVOLUTION:
			case GN_CART_TORNADO:
			case GN_CARTCANNON:
				if( sd && pc_checkskill(sd, GN_REMODELING_CART) > 0 )
					hitrate += 4 * pc_checkskill(sd, GN_REMODELING_CART);
				break;
			case GC_VENOMPRESSURE:
				hitrate += 10 + 4 * skill_lv;
				break;
			case SC_FATALMENACE:
				hitrate -= 35 - 5 * skill_lv;
				break;
			case LG_BANISHINGPOINT:
				hitrate += 3 * skill_lv;
				break;
			case RL_SLUGSHOT:
				if ( distance_bl(src,target) > 3 )
					hitrate -= (11 - skill_lv) * (distance_bl(src,target) - 3);
				break;
		}

		// Weaponry Research hidden bonus
		if (sd && (skill = pc_checkskill(sd,BS_WEAPONRESEARCH)) > 0)
			hitrate += hitrate * ( 2 * skill ) / 100;

		if( sd && (sd->status.weapon == W_DAGGER || sd->status.weapon == W_1HSWORD) && 
			(skill = pc_checkskill(sd, GN_TRAINING_SWORD)) > 0 )
			hitrate += 3 * skill;

		hitrate = cap_value(hitrate, battle_config.min_hitrate, battle_config.max_hitrate); 

		if(rand()%100 >= hitrate)
			wd.dmg_lv = ATK_FLEE;
		else
			flag.hit = 1;
	}	//End hit/miss calculation

	if (flag.hit && !flag.infdef) //No need to do the math for plants
	{	//Hitting attack

//Assuming that 99% of the cases we will not need to check for the flag.rh... we don't.
//ATK_RATE scales the damage. 100 = no change. 50 is halved, 200 is doubled, etc
#define ATK_RATE( a ) { wd.damage= wd.damage*(a)/100 ; if(flag.lh) wd.damage2= wd.damage2*(a)/100; }
#define ATK_RATE2( a , b ) { wd.damage= wd.damage*(a)/100 ; if(flag.lh) wd.damage2= wd.damage2*(b)/100; }
//Adds dmg%. 100 = +100% (double) damage. 10 = +10% damage
#define ATK_ADDRATE( a ) { wd.damage+= wd.damage*(a)/100 ; if(flag.lh) wd.damage2+= wd.damage2*(a)/100; }
#define ATK_ADDRATE2( a , b ) { wd.damage+= wd.damage*(a)/100 ; if(flag.lh) wd.damage2+= wd.damage2*(b)/100; }
//Adds an absolute value to damage. 100 = +100 damage
#define ATK_ADD( a ) { wd.damage+= a; if (flag.lh) wd.damage2+= a; }
#define ATK_ADD2( a , b ) { wd.damage+= a; if (flag.lh) wd.damage2+= b; }

		switch (skill_num)
		{	//Calc base damage according to skill
			case NJ_ISSEN:
				wd.damage = 40*sstatus->str +skill_lv*(sstatus->hp/10 + 35);
				wd.damage2 = 0;
				status_set_hp(src, 1, 0);
				break;
			case PA_SACRIFICE:
				wd.damage = sstatus->max_hp* 9/100;
				wd.damage2 = 0;
				break;
			case LK_SPIRALPIERCE:
			case ML_SPIRALPIERCE:
				if (sd) {
					short index = sd->equip_index[EQI_HAND_R];

					if (index >= 0 &&
						sd->inventory_data[index] &&
						sd->inventory_data[index]->type == IT_WEAPON)
						wd.damage = sd->inventory_data[index]->weight*8/100; //80% of weight
					if( sc && sc->data[SC_GLOOMYDAY_SK] )
						wd.damage += wd.damage * (15 + rand()%sc->data[SC_GLOOMYDAY_SK]->val3) / 100;
				} else
					wd.damage = sstatus->rhw.atk2*8/10; //Else use Atk2

				ATK_ADDRATE(50*skill_lv); //Skill modifier applies to weight only.
				i = sstatus->str/10;
				i*=i;
				ATK_ADD(i); //Add str bonus.
				switch (tstatus->size) { //Size-fix. Is this modified by weapon perfection?
					case 0: //Small: 125%
						ATK_RATE(125);
						break;
					//case 1: //Medium: 100%
					case 2: //Large: 75%
						ATK_RATE(75);
						break;
				}
				break;
			case CR_SHIELDBOOMERANG:
			case PA_SHIELDCHAIN:
				wd.damage = sstatus->batk;
				if( sd )
				{
					int damagevalue = 0;
					short index = sd->equip_index[EQI_HAND_L];
					if( index >= 0 && sd->inventory_data[index] && sd->inventory_data[index]->type == IT_ARMOR )
						damagevalue = sd->inventory_data[index]->weight/10;
					if( sc && sc->data[SC_GLOOMYDAY_SK] )
						damagevalue += damagevalue * (15 + rand()%sc->data[SC_GLOOMYDAY_SK]->val3) / 100;
					ATK_ADD(damagevalue);
				}
				else
					ATK_ADD(sstatus->rhw.atk2); //Else use Atk2
				break;
		case RK_DRAGONBREATH:
		case RK_DRAGONBREATH_WATER:
			{
				int damagevalue = 0;
 				if( battle_config.skillsbonus_maxhp_RK && sstatus->hp > battle_config.skillsbonus_maxhp_RK ) // [Pinky]
					damagevalue = ((battle_config.skillsbonus_maxhp_RK / 50) + (status_get_max_sp(src) / 4)) * skill_lv;
 				else
					damagevalue = ((sstatus->hp / 50) + (status_get_max_sp(src) / 4)) * skill_lv;
				if ( re_baselv_bonus == 1 && s_level >= 100 )
					damagevalue = damagevalue * s_level / 150;// Base level bonus.
				if (sd) damagevalue = damagevalue * (100 + 5 * (pc_checkskill(sd,RK_DRAGONTRAINING) - 1)) / 100;
				ATK_ADD(damagevalue);
			}
			break;
			case NC_SELFDESTRUCTION:
				{
					int damagevalue = 0;
					wd.damage = 0;
					damagevalue = (1 + skill_lv) * (8 + pc_checkskill(sd,NC_MAINFRAME)) * (status_get_sp(src) + sstatus->vit);
					if ( re_baselv_bonus == 1 && s_level >= 100 )
						damagevalue = damagevalue * s_level / 100;// Base level bonus.
					damagevalue = damagevalue + sstatus->hp;
					ATK_ADD(damagevalue);
					if (sd) status_set_sp(src, 0, 0);
				}
				break;
			case KO_HAPPOKUNAI:
				if( sd )
				{
					short index = sd->equip_index[EQI_AMMO];
					wd.damage = 0;
					if( index >= 0 && sd->inventory_data[index] && sd->inventory_data[index]->type == IT_AMMO )
						ATK_ADD((3 * (2 * sstatus->batk + sstatus->rhw.atk + sd->inventory_data[index]->atk)) * (skill_lv + 5) / 5);
				}
				else
					ATK_ADD(5000);
				break;
			case HFLI_SBR44:	//[orn]
				if(src->type == BL_HOM) {
					wd.damage = ((TBL_HOM*)src)->homunculus.intimacy ;
					break;
				}
			default:
			{
				i = (flag.cri?1:0)|
					(flag.arrow?2:0)|
					(skill_num == HW_MAGICCRASHER?4:0)|
					(!skill_num && sc && sc->data[SC_CHANGE]?4:0)|
					(skill_num == MO_EXTREMITYFIST?8:0)|
					(sc && sc->data[SC_WEAPONPERFECTION]?8:0);
				if (flag.arrow && sd)
				switch(sd->status.weapon) {
					case W_BOW:
					case W_REVOLVER:
					case W_GATLING:
					case W_SHOTGUN:
					case W_GRENADE:
						break;
					default:
						i |= 16; // for ex. shuriken must not be influenced by DEX
				}
				wd.damage = battle_calc_base_damage(sstatus, &sstatus->rhw, sc, tstatus->size, sd, i);
				if (flag.lh)
					wd.damage2 = battle_calc_base_damage(sstatus, &sstatus->lhw, sc, tstatus->size, sd, i);

				if (nk&NK_SPLASHSPLIT){ // Divide ATK among targets
					if(wflag>0)
						wd.damage/= wflag;
					else
						ShowError("0 enemies targeted by %d:%s, divide per 0 avoided!\n", skill_num, skill_get_name(skill_num));
				}

				//Add any bonuses that modify the base baseatk+watk (pre-skills)
				if( sd )
				{
					// Guillotine Cross skills that have their damage boosted by EDP should be handled in a renewal way.
					// Note: This is as close to a renewal handleing as I can do for these skills. However, since their
					// was no such thing as EDP making certain assassin skills do half the damage in pre-renewal, the
					// Guillotine Cross skills that would normally deal half damage under EDP in renewal will instead be
					// set to not be affected by EDP at all. The 2 skills are Cross Impact and Counter Slash. [Rytech]
					if (sc && sc->data[SC_EDP] && (skill_num == GC_DARKILLUSION || skill_num == GC_WEAPONCRUSH || 
						skill_num == GC_VENOMPRESSURE || skill_num == GC_PHANTOMMENACE || skill_num == GC_ROLLINGCUTTER || 
						skill_num == GC_CROSSRIPPERSLASHER))
					{
						int edpva = battle_config.gc_skill_edp_boost_formula_a;
						int edpvb = battle_config.gc_skill_edp_boost_formula_b;
						if ( battle_config.gc_skill_edp_boost_formula_c == 1 )
						{ATK_ADDRATE(edpva + edpvb * sc->data[SC_EDP]->val1);}
						else
						{ATK_ADDRATE(edpva - edpvb * sc->data[SC_EDP]->val1);}
					}

					if( sd->atk_rate )
						ATK_ADDRATE(sd->atk_rate);

					if(flag.cri && sd->crit_atk_rate)
						ATK_ADDRATE(sd->crit_atk_rate);

					if( sd->status.party_id && (skill=pc_checkskill(sd,TK_POWER)) > 0 )
					{
						if( (i = party_foreachsamemap(party_sub_count, sd, 0)) > 1 ) // exclude the player himself [Inkfish]
							ATK_ADDRATE(2*skill*i);
					}
				}
				break;
			}	//End default case
		} //End switch(skill_num)

		//Skill damage modifiers that stack linearly
		if(sc && skill_num != PA_SACRIFICE)
		{
			if(sc->data[SC_OVERTHRUST])
				skillratio += sc->data[SC_OVERTHRUST]->val3;
			if(sc->data[SC_MAXOVERTHRUST])
				skillratio += sc->data[SC_MAXOVERTHRUST]->val2;
			if(sc->data[SC_BERSERK])
				skillratio += 100;
		}
		if( !skill_num )
		{ // Random chance to deal multiplied damage - Consider it as part of skill-based-damage
			if( sd && sd->random_attack_increase_add > 0 && sd->random_attack_increase_per && rand()%100 < sd->random_attack_increase_per )
				skillratio += sd->random_attack_increase_add;

			ATK_RATE(skillratio);
		}
		else
		{
			switch( skill_num )
			{
				case SM_BASH:
				case MS_BASH:
					skillratio += 30*skill_lv;
					break;
				case SM_MAGNUM:
				case MS_MAGNUM:
					skillratio += 20*skill_lv; 
					break;
				case MC_MAMMONITE:
					skillratio += 50*skill_lv;
					break;
				case HT_POWER:
					skillratio += -50+8*sstatus->str;
					break;
				case AC_DOUBLE:
				case MA_DOUBLE:
					skillratio += 10*(skill_lv-1);
					break;
				case AC_SHOWER:
				case MA_SHOWER:
					skillratio += 5*skill_lv-25;
					break;
				case AC_CHARGEARROW:
				case MA_CHARGEARROW:
					skillratio += 50;
					break;
				case HT_FREEZINGTRAP:
				case MA_FREEZINGTRAP:
					skillratio += -50+10*skill_lv;
					break;
				case KN_PIERCE:
				case ML_PIERCE:
					skillratio += 10*skill_lv;
					break;
				case MER_CRASH:
					skillratio += 10*skill_lv;
					break;
				case KN_SPEARSTAB:
					skillratio += 15*skill_lv;
					break;
				case KN_SPEARBOOMERANG:
					skillratio += 50*skill_lv;
					break;
				case KN_BRANDISHSPEAR:
				case ML_BRANDISH:
				{
					int ratio = 100+20*skill_lv;
					skillratio += ratio-100;
					if(skill_lv>3 && wflag==1) skillratio += ratio/2;
					if(skill_lv>6 && wflag==1) skillratio += ratio/4;
					if(skill_lv>9 && wflag==1) skillratio += ratio/8;
					if(skill_lv>6 && wflag==2) skillratio += ratio/2;
					if(skill_lv>9 && wflag==2) skillratio += ratio/4;
					if(skill_lv>9 && wflag==3) skillratio += ratio/2;
					if( sc && sc->data[SC_GLOOMYDAY_SK] )
						skillratio += skillratio * (15 + rand()%sc->data[SC_GLOOMYDAY_SK]->val3) / 100;
					break;
				}
				case KN_BOWLINGBASH:
				case MS_BOWLINGBASH:
					skillratio+= 40*skill_lv;
					break;
				case AS_GRIMTOOTH:
					skillratio += 20*skill_lv;
					break;
				case AS_POISONREACT:
					skillratio += 30*skill_lv;
					break;
				case AS_SONICBLOW:
					skillratio += -50+5*skill_lv;
					break;
				case TF_SPRINKLESAND:
					skillratio += 30;
					break;
				case MC_CARTREVOLUTION:
					skillratio += 50;
					if(sd && sd->cart_weight)
						skillratio += 100*sd->cart_weight/battle_config.max_cart_weight; // +1% every 1% weight
					else if (!sd)
						skillratio += 100; //Max damage for non players.
					break;
				case NPC_RANDOMATTACK:
					skillratio += 100 * skill_lv;
					break;
				case NPC_WATERATTACK:
				case NPC_GROUNDATTACK:
				case NPC_FIREATTACK:
				case NPC_WINDATTACK:
				case NPC_POISONATTACK:
				case NPC_HOLYATTACK:
				case NPC_DARKNESSATTACK:
				case NPC_UNDEADATTACK:
				case NPC_TELEKINESISATTACK:
				case NPC_BLOODDRAIN:
				case NPC_ACIDBREATH:
				case NPC_DARKNESSBREATH:
				case NPC_FIREBREATH:
				case NPC_ICEBREATH:
				case NPC_THUNDERBREATH:
				case NPC_HELLJUDGEMENT:
				case NPC_PULSESTRIKE:
					skillratio += 100*(skill_lv-1);
					break;
				case RG_BACKSTAP:
					if(sd && sd->status.weapon == W_BOW && battle_config.backstab_bow_penalty)
						skillratio += (200+40*skill_lv)/2;
					else
						skillratio += 200+40*skill_lv;
					break;
				case RG_RAID:
					skillratio += 40*skill_lv;
					break;
				case RG_INTIMIDATE:
					skillratio += 30*skill_lv;
					break;
				case CR_SHIELDCHARGE:
					skillratio += 20*skill_lv;
					if( sc && sc->data[SC_GLOOMYDAY_SK] )
						skillratio += skillratio * (15 + rand()%sc->data[SC_GLOOMYDAY_SK]->val3) / 100;
					break;
				case CR_SHIELDBOOMERANG:
					skillratio += 30*skill_lv;
					if( sc && sc->data[SC_GLOOMYDAY_SK] )
						skillratio += skillratio * (15 + rand()%sc->data[SC_GLOOMYDAY_SK]->val3) / 100;
					break;
				case NPC_DARKCROSS:
				case CR_HOLYCROSS:
					skillratio += 35*skill_lv;
					break;
				case AM_DEMONSTRATION:
					skillratio += 20*skill_lv;
					break;
				case AM_ACIDTERROR:
					skillratio += 40*skill_lv;
					break;
				case MO_FINGEROFFENSIVE:
					skillratio+= 50 * skill_lv;
					break;
				case MO_INVESTIGATE:
					skillratio += 75*skill_lv;
					flag.pdef = flag.pdef2 = 2;
					break;
				case MO_EXTREMITYFIST:
					{	//Overflow check. [Skotlex]
						unsigned int ratio = skillratio + 100*(8 + sstatus->sp/10);
						//You'd need something like 6K SP to reach this max, so should be fine for most purposes.
						if (ratio > 60000) ratio = 60000; //We leave some room here in case skillratio gets further increased.
						skillratio = (unsigned short)ratio;
						status_set_sp(src, 0, 0);
					}
					break;
				case MO_TRIPLEATTACK:
					skillratio += 20*skill_lv;
					break;
				case MO_CHAINCOMBO:
					skillratio += 50+50*skill_lv;
					break;
				case MO_COMBOFINISH:
					skillratio += 140+60*skill_lv;
					break;
				case BA_MUSICALSTRIKE:
				case DC_THROWARROW:
					skillratio += 25+25*skill_lv;
					break;
				case CH_TIGERFIST:
					skillratio += 100*skill_lv-60;
					break;
				case CH_CHAINCRUSH:
					skillratio += 300+100*skill_lv;
					break;
				case CH_PALMSTRIKE:
					skillratio += 100+100*skill_lv;
					break;
				case LK_HEADCRUSH:
					skillratio += 40*skill_lv;
					break;
				case LK_JOINTBEAT:
					i = 10*skill_lv-50;
					// Although not clear, it's being assumed that the 2x damage is only for the break neck ailment.
					if (wflag&BREAK_NECK) i*=2;
					skillratio += i;
					break;
				case ASC_METEORASSAULT:
					skillratio += 40*skill_lv-60;
					break;
				case SN_SHARPSHOOTING:
				case MA_SHARPSHOOTING:
					skillratio += 100 + 50 * skill_lv;
					break;
				case CG_ARROWVULCAN:
					skillratio += 100+100*skill_lv;
					break;
				case AS_SPLASHER:
					skillratio += 400+50*skill_lv;
					if(sd)
						skillratio += 30 * pc_checkskill(sd,AS_POISONREACT);
					break;
				case ASC_BREAKER:
					skillratio += 100*skill_lv-100;
					break;
				case PA_SACRIFICE:
					skillratio += 10*skill_lv-10;
					break;
				case PA_SHIELDCHAIN:
					skillratio += 30*skill_lv;
					if( sc && sc->data[SC_GLOOMYDAY_SK] )
						skillratio += skillratio * (15 + rand()%sc->data[SC_GLOOMYDAY_SK]->val3) / 100;
					break;
				case WS_CARTTERMINATION:
					i = 10 * (16 - skill_lv);
					if (i < 1) i = 1;
					//Preserve damage ratio when max cart weight is changed.
					if(sd && sd->cart_weight)
						skillratio += sd->cart_weight/i * 80000/battle_config.max_cart_weight - 100;
					else if (!sd)
						skillratio += 80000 / i - 100;
					break;
				case TK_DOWNKICK:
					skillratio += 60 + 20*skill_lv;
					break;
				case TK_STORMKICK:
					skillratio += 60 + 20*skill_lv;
					break;
				case TK_TURNKICK:
					skillratio += 90 + 30*skill_lv;
					break;
				case TK_COUNTER:
					skillratio += 90 + 30*skill_lv;
					break;
				case TK_JUMPKICK:
					skillratio += -70 + 10*skill_lv;
					if (sc && sc->data[SC_COMBO] && sc->data[SC_COMBO]->val1 == skill_num)
						skillratio += 10*s_level/3; //Tumble bonus
					if (wflag)
					{
						skillratio += 10*s_level/3; //Running bonus (TODO: What is the real bonus?)
						if( sc && sc->data[SC_SPURT] )  // Spurt bonus
							skillratio *= 2;
					}
					break;
				case GS_TRIPLEACTION:
					skillratio += 50*skill_lv;
					break;
				case GS_BULLSEYE:
					//Only works well against brute/demihumans non bosses.
					if((tstatus->race == RC_BRUTE || tstatus->race == RC_DEMIHUMAN)
						&& !(tstatus->mode&MD_BOSS))
						skillratio += 400;
					break;
				case GS_TRACKING:
					skillratio += 100 *(skill_lv+1);
					break;
				case GS_PIERCINGSHOT:
					skillratio += 20*skill_lv;
					break;
				case GS_RAPIDSHOWER:
					skillratio += 10*skill_lv;
					break;
				case GS_DESPERADO:
					skillratio += 50*(skill_lv-1);
					break;
				case GS_DUST:
					skillratio += 50*skill_lv;
					break;
				case GS_FULLBUSTER:
					skillratio += 100*(skill_lv+2);
					break;
				case GS_SPREADATTACK:
					skillratio += 20*(skill_lv-1);
					break;
				case NJ_HUUMA:
					skillratio += 50 + 150*skill_lv;
					break;
				case NJ_TATAMIGAESHI:
					skillratio += 10*skill_lv;
					break;
				case NJ_KASUMIKIRI:
					skillratio += 10*skill_lv;
					break;
				case NJ_KIRIKAGE:
					skillratio += 100*(skill_lv-1);
					break;
				case KN_CHARGEATK:
					{
					int k = (wflag-1)/3; //+100% every 3 cells of distance
					if( k > 2 ) k = 2; // ...but hard-limited to 300%.
					skillratio += 100 * k; 
					}
					break;
				case HT_PHANTASMIC:
					skillratio += 50;
					break;
				case MO_BALKYOUNG:
					skillratio += 200;
					break;
				case HFLI_MOON:	//[orn]
					skillratio += 10+110*skill_lv;
					break;
				case HFLI_SBR44:	//[orn]
					skillratio += 100 *(skill_lv-1);
					break;
				case NPC_VAMPIRE_GIFT:
					skillratio += ((skill_lv-1)%5+1)*100;
					break;
				case RK_SONICWAVE:
					skillratio += 400 + 100 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio += skillratio * (s_level - 100) / 200;	// Base level bonus.
					break;
				case RK_HUNDREDSPEAR:
					skillratio += 500 + 80 * skill_lv + 50 * pc_checkskill(sd,LK_SPIRALPIERCE);
					if( sd )
					{
						short index = sd->equip_index[EQI_HAND_R];
						if( index >= 0 && sd->inventory_data[index] && sd->inventory_data[index]->type == IT_WEAPON )
						{
							short spearwbd = 1000 - sd->inventory_data[index]->weight / 10;// Spear Weight Bonus Damage.
							if ( spearwbd < 0 )
							spearwbd = 0;// If weight of weapon is more then 1000, bonus is set to 0 to prevent negative value. [Rytech]
							skillratio += spearwbd;
						}
					}
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio += skillratio * (s_level - 100) / 200;	// Base level bonus.
					if( sc && sc->data[SC_GLOOMYDAY_SK] )
						skillratio += skillratio * (15 + rand()%sc->data[SC_GLOOMYDAY_SK]->val3) / 100;
					break;
				case RK_WINDCUTTER:
					skillratio += 50 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case RK_IGNITIONBREAK:
					i = distance_bl(src,target);
					if( i < 2 ) skillratio = 300 * skill_lv;
					else
					if( i < 4 ) skillratio = 250 * skill_lv;
					else
					skillratio = 200 * skill_lv;
					if( sstatus->rhw.ele == ELE_FIRE )	skillratio +=  100 * skill_lv;	// Bonus by fire element endow.
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio += skillratio * (s_level - 100) / 100;	// Base level bonus.
					break;
				case RK_CRUSHSTRIKE:
					if( sd )
					{
						short index = sd->equip_index[EQI_HAND_R];
						if( index >= 0 && sd->inventory_data[index] && sd->inventory_data[index]->type == IT_WEAPON )
							skillratio = sd->inventory_data[index]->wlv * (sd->status.inventory[index].refine + 6) * 100 + sd->inventory_data[index]->atk + sd->inventory_data[index]->weight / 10;
					}
					break;
				case RK_STORMBLAST:
					skillratio = 100 * sstatus->int_ / 8 + 100 * (sd ? pc_checkskill(sd,RK_RUNEMASTERY) : 10);
					break;
				case RK_PHANTOMTHRUST:
					skillratio = 50 * skill_lv + 10 * pc_checkskill(sd,KN_SPEARMASTERY);
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 150;
					break;
				case GC_CROSSIMPACT:
					skillratio += 900 + 100 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 120;
					break;
				case GC_COUNTERSLASH:
					skillratio += 200 + 100 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 120;
					if( re_baselv_bonus == 1 && s_level >= 100 )
					skillratio += sstatus->agi * 2 + status_get_job_lv(src) * 4;
					else
					skillratio += sstatus->agi * 2 + 200;
					break;
				case GC_PHANTOMMENACE:
					skillratio += 200;
					break;
				case GC_ROLLINGCUTTER:
					skillratio = 50 + 50 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;
					break;
				case GC_CROSSRIPPERSLASHER:
					skillratio += 300 + 80 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;
					if( sc && sc->data[SC_ROLLINGCUTTER] )
						skillratio += sc->data[SC_ROLLINGCUTTER]->val1 * sstatus->agi;
					break;
				case AB_DUPLELIGHT_MELEE:
					skillratio += 10 * skill_lv;
					break;
				case RA_ARROWSTORM:
					skillratio += 900 + 80 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case RA_AIMEDBOLT:
					skillratio += 400 + 50 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					if( tsc && (tsc->data[SC_BITE] || tsc->data[SC_ANKLE] || tsc->data[SC_ELECTRICSHOCKER]) )
						wd.div_ = tstatus->size + 2 + rand()%2;
					break;
				case RA_CLUSTERBOMB:
					skillratio += 100 + 100 * skill_lv;
					break;
				case RA_WUGDASH:
					skillratio = 300;
					if( sc && sc->data[SC_DANCEWITHWUG] )
						skillratio += 10 * sc->data[SC_DANCEWITHWUG]->val1 * (2 + chorusbonus);
					break;
				case RA_WUGSTRIKE:
					skillratio = 200 * skill_lv;
					if( sc && sc->data[SC_DANCEWITHWUG] )
						skillratio += 10 * sc->data[SC_DANCEWITHWUG]->val1 * (2 + chorusbonus);
					break;
				case RA_WUGBITE:
					skillratio += 300 + 200 * skill_lv;
					if ( skill_lv == 5 ) skillratio += 100;
					if( sc && sc->data[SC_DANCEWITHWUG] )
						skillratio += 10 * sc->data[SC_DANCEWITHWUG]->val1 * (2 + chorusbonus);
					break;
				case RA_SENSITIVEKEEN:
					skillratio += 50 * skill_lv;
					break;
				case NC_BOOSTKNUCKLE:
					skillratio += 100 + 100 * skill_lv + sstatus->dex;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 120;	// Base level bonus.
					break;
				case NC_PILEBUNKER:
					skillratio += 200 + 100 * skill_lv + sstatus->str;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case NC_VULCANARM:
					skillratio = 70 * skill_lv + sstatus->dex;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 120;	// Base level bonus.
					break;
				case NC_FLAMELAUNCHER:
				case NC_COLDSLOWER:
					skillratio += 200 + 300 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 150;	// Base level bonus.
					break;
				case NC_ARMSCANNON:
					switch( tstatus->size )
					{
						case 0: skillratio += 200 + 400 * skill_lv; break;// Small
						case 1: skillratio += 200 + 350 * skill_lv; break;// Medium
						case 2: skillratio += 200 + 300 * skill_lv; break;// Large
					}
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 120;	// Base level bonus.
					//NOTE: Their's some other factors that affects damage, but not sure how exactly. Will recheck one day. [Rytech]
					break;
				case NC_AXEBOOMERANG:
					skillratio += 150 + 50 * skill_lv;
					if( sd )
					{
						short index = sd->equip_index[EQI_HAND_R];
						if( index >= 0 && sd->inventory_data[index] && sd->inventory_data[index]->type == IT_WEAPON )
						skillratio += sd->inventory_data[index]->weight / 10;// Weight is divided by 10 since 10 weight in coding make 1 whole actural weight. [Rytech]
					}
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case NC_POWERSWING:
					skillratio += 200 + 100 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio += (sstatus->str + sstatus->dex) * s_level / 100;	// Base level bonus.
					else
						skillratio += sstatus->str + sstatus->dex;
					break;
				case NC_AXETORNADO:
					skillratio += 100 + 100 * skill_lv + sstatus->vit;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					if( sstatus->rhw.ele == ELE_WIND ) skillratio +=  skillratio * 25 / 100;// 1.25x Damage if the weapon is wind element. [Rytech]
					i = distance_bl(src,target);
					if( i > 2 ) skillratio = skillratio * 75 / 100;
					break;
				case SC_FATALMENACE:
					skillratio += 100 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case SC_TRIANGLESHOT:
					skillratio += 200 + (skill_lv - 1) * sstatus->agi / 2;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 120;	// Base level bonus.
					break;
				case SC_FEINTBOMB:
					skillratio = (1 + skill_lv) * sstatus->dex / 2;
					if( re_baselv_bonus == 1 && s_level >= 100 )
					skillratio = skillratio * status_get_job_lv(src) / 10;
					else
					skillratio = skillratio * 5;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 120;	// Base level bonus.
					break;
				case LG_CANNONSPEAR:
					skillratio = (50 + sstatus->str) * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case LG_BANISHINGPOINT:
					skillratio = (50 * skill_lv) + 30 * (sd ? pc_checkskill(sd,SM_BASH) : 10);
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case LG_SHIELDPRESS:
					skillratio = 150 * skill_lv + sstatus->str;
					if( sd )
					{
						short index = sd->equip_index[EQI_HAND_L];
						if( index >= 0 && sd->inventory_data[index] && sd->inventory_data[index]->type == IT_ARMOR )
						skillratio += sd->inventory_data[index]->weight / 10;
					}
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					if( sc && sc->data[SC_GLOOMYDAY_SK] )
						skillratio += skillratio * (15 + rand()%sc->data[SC_GLOOMYDAY_SK]->val3) / 100;
					break;
				case LG_PINPOINTATTACK:
					skillratio = 100 * skill_lv + 5 * sstatus->agi;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 120;	// Base level bonus.
					break;
				case LG_RAGEBURST:
					if( sd && sd->rageball_old )
						skillratio = 200 * sd->rageball_old + (sstatus->max_hp - sstatus->hp) / 100;
					else//If your a GM
						skillratio = 3000 + (sstatus->max_hp - sstatus->hp) / 100;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case LG_SHIELDSPELL:
					if ( sd && skill_lv == 1 )
						{
						struct item_data *shield_data = sd->inventory_data[sd->equip_index[EQI_HAND_L]];
						if( shield_data )
							if( battle_config.renewal_baselvl_skill_effect == 1 && status_get_lv(src) >= 100 )
								skillratio = 4 * s_level + 100 * shield_data->def + 2 * sstatus->vit;
							else
								skillratio = 600 + 100 * shield_data->def + 2 * sstatus->vit;
						}
					else
						skillratio = 0;//Prevents ATK damage from being done on LV 2 usage since LV 2 us MATK. [Rytech]
					break;
				case LG_OVERBRAND:
					skillratio = 400 * skill_lv + 50 * pc_checkskill(sd,CR_SPEARQUICKEN);
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 150;	// Base level bonus.
					break;
				case LG_OVERBRAND_BRANDISH:
					skillratio = 300 * skill_lv + sstatus->str + sstatus->dex;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 150;	// Base level bonus.
					break;
				case LG_OVERBRAND_PLUSATK:
					skillratio = 200 * skill_lv;
					break;
				case LG_MOONSLASHER:
					skillratio = 120 * skill_lv + 80 * pc_checkskill(sd,LG_OVERBRAND);
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case LG_RAYOFGENESIS:
					skillratio += 200 + 300 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case LG_EARTHDRIVE:
					if( sd )
					{
						short index = sd->equip_index[EQI_HAND_L];
						if( index >= 0 && sd->inventory_data[index] && sd->inventory_data[index]->type == IT_ARMOR )
						skillratio = (1 + skill_lv) * sd->inventory_data[index]->weight / 10;
					}
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case LG_HESPERUSLIT:
					skillratio = 120 * skill_lv;
					if( sc && sc->data[SC_BANDING] )
						skillratio += 200 * sc->data[SC_BANDING]->val2;
					if( sc && sc->data[SC_BANDING] && sc->data[SC_BANDING]->val2 > 5 )
						skillratio = skillratio * 150 / 100;
					if( sc && sc->data[SC_INSPIRATION] )
						skillratio += 600;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case SR_DRAGONCOMBO:
					skillratio += 40 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case SR_SKYNETBLOW:
					if( sc && sc->data[SC_COMBO] )//This part of the code wont entirely work until I update the combo system. [Rytech]
					skillratio = 100 * skill_lv + sstatus->agi + 150;
					else
					skillratio = 80 * skill_lv + sstatus->agi;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case SR_EARTHSHAKER:
					{
						struct status_change *tsc = status_get_sc(target);//Should check for Shadow Chaser's Invisiable skill right? [Rytech]
						if( tsc && (tsc->data[SC_HIDING] || tsc->data[SC_CLOAKING] || tsc->data[SC_CHASEWALK] || 
							tsc->data[SC_CLOAKINGEXCEED] || tsc->data[SC_CAMOUFLAGE] || tsc->data[SC_INVISIBILITY]) )
						{
						skillratio = 150 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						skillratio = skillratio + 3 * sstatus->int_;
						}
						else
						{
						skillratio = 50 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						skillratio = skillratio + 2 * sstatus->int_;
						}
					}
					break;
				case SR_FALLENEMPIRE:
					skillratio += 150 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 150;	// Base level bonus.
					break;
				case SR_TIGERCANNON:
					if( sc && sc->data[SC_COMBO] )//This part of the code wont entirely work until I update the combo system. [Rytech]
					skillratio = (sstatus->max_hp * ( 10 + 2 * skill_lv ) / 100 + sstatus->max_sp * ( 5 + 1 * skill_lv ) / 100) / 2;
					else
					skillratio = (sstatus->max_hp * ( 10 + 2 * skill_lv ) / 100 + sstatus->max_sp * ( 5 + 1 * skill_lv ) / 100) / 4;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case SR_RAMPAGEBLASTER:
					if( sc && sc->data[SC_EXPLOSIONSPIRITS] )
						skillratio = 20 * (skill_lv + pc_checkskill(sd,MO_EXPLOSIONSPIRITS)) * sd->spiritball_old;
					else
						skillratio = 20 * skill_lv * sd->spiritball_old;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 150;	// Base level bonus.
					break;
				case SR_CRESCENTELBOW_AUTOSPELL://Will not work until I can recode this skill in another update. [Rytech]
					skillratio = tstatus->hp / 100 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 125;	// Base level bonus.
					if ( skillratio > 5000 )
						skillratio = 5000;//Ratio is officially capped at 5000%.
					break;
				case SR_KNUCKLEARROW:
					if( wflag&4 )//Bonus damage if knocked back into a wall.
					{
					if ( tsd )//Players have weight. Monster's dont.
					{if( re_baselv_bonus == 1 && s_level >= 100 )
					skillratio = 150 * skill_lv + 1000 * (tsd->weight / 10) / tsd->max_weight + 5 * status_get_lv(target);
					else
					skillratio = 150 * skill_lv + 1000 * (tsd->weight / 10) / tsd->max_weight + 750;}
					else//Knockback damage on mosnters. Need official formula for mobs since they have no weight.
					skillratio = 150 * skill_lv + 1000 + 5 * status_get_lv(target);
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 150;	// Base level bonus.
					}
					else
					{//Main damage from skill use.
					skillratio += 400 + (100 * skill_lv);
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					}
					break;
				case SR_WINDMILL:
					if( re_baselv_bonus == 1 && s_level >= 100 )
					skillratio = s_level + sstatus->dex;
					else
					skillratio = 150 + sstatus->dex;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case SR_GATEOFHELL:
					if( sc && sc->data[SC_COMBO] )//Wont work until the combo system is fixed for this skill.
					skillratio = 800 * skill_lv;
					else
					skillratio = 500 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case SR_GENTLETOUCH_QUIET:
					skillratio = 100 * skill_lv + sstatus->dex;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case SR_HOWLINGOFLION:
					skillratio = 300 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 150;	// Base level bonus.
					break;
				case SR_RIDEINLIGHTNING:
					{
					int windbonus = 0;
					if( sstatus->rhw.ele == ELE_WIND ) windbonus = 50 * skill_lv;
					skillratio = 200 * skill_lv + windbonus;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					}
					break;
				case WM_REVERBERATION_MELEE:
					skillratio += 200 + 100 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					if (wflag <= 0)// wflag holds the number of enemy targets in the AoE to divide the damage between.
						wflag = 1;// Make sure its always at least 1 to avoid divide by 0 crash.
					skillratio = skillratio / wflag;
					break;
				case WM_SEVERE_RAINSTORM_MELEE:
					skillratio = (sstatus->agi + sstatus->dex) * skill_lv / 5;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case WM_GREAT_ECHO:
					skillratio += 300 + 200 * skill_lv;
					if ( chorusbonus == 1 )//Chorus bonus dont count the first 2 Minstrel's/Wanderer's and only increases when their's 3 or more. [Rytech]
					skillratio += 100;
					else if ( chorusbonus == 2 )
					skillratio += 200;
					else if ( chorusbonus == 3 )
					skillratio += 400;
					else if ( chorusbonus == 4 )
					skillratio += 800;
					else if ( chorusbonus == 5 )
					skillratio += 1600;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					break;
				case SO_VARETYR_SPEAR:
					skillratio = 50 * skill_lv + 50 * pc_checkskill(sd, SO_STRIKING);
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					if( sc && sc->data[SC_BLAST_OPTION] )
						skillratio += skillratio * sc->data[SC_BLAST_OPTION]->val2 / 100;
					break;
				case GN_CART_TORNADO:
					{
					int strbonus = sstatus->str;//Supposed to take only base STR, but current code wont allow that. So well just take STR for now. [Rytech]
					if ( strbonus > 120 )//Max base stat limit on official is 120. So well allow no higher then 120 STR here. This limit prevents
						strbonus = 120;//the division from going any lower then 30 so the server wont divide by 0 if someone has 150 STR.
					skillratio = 50 * skill_lv + sd->cart_weight / 10 / (150 - strbonus) + 50 * pc_checkskill(sd, GN_REMODELING_CART);
					}
					break;
				case GN_CARTCANNON:
					skillratio = 60 * skill_lv + 50 * pc_checkskill(sd, GN_REMODELING_CART) * sstatus->int_ / 40;
					break;
				case GN_SPORE_EXPLOSION:
						skillratio = 100 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio += ( 200 + sstatus->int_ ) * s_level / 100;	// Base level bonus.
					else
						skillratio += 200 + sstatus->int_;
						break;
				case GN_WALLOFTHORN:
					skillratio += 10 * skill_lv;
					break;
				case GN_CRAZYWEED_ATK:
					skillratio += 400 + 100 * skill_lv;
					break;
				case GN_SLINGITEM_RANGEMELEEATK:
					if( sd )
					{
						switch( sd->itemid )
						{
							case ITEMID_APPLE_BOMB:
								skillratio = 300 + sstatus->str + sstatus->dex;
								break;
							case ITEMID_COCONUT_BOMB:
							case ITEMID_PINEAPPLE_BOMB:
								skillratio = 800 + sstatus->str + sstatus->dex;
								break;
							case ITEMID_MELON_BOMB:
								skillratio = 500 + sstatus->str + sstatus->dex;
								break;
							case ITEMID_BANANA_BOMB:
								skillratio = 877 + sstatus->str + sstatus->dex;
								break;
							case ITEMID_DARK_LUMP:
								skillratio = (sstatus->str + sstatus->agi + sstatus->dex) / 3;
								break;
							case ITEMID_HARD_DARK_LUMP:
								skillratio = (sstatus->str + sstatus->agi + sstatus->dex) / 2;
								break;
							case ITEMID_VERY_HARD_DARK_LUMP:
								skillratio = sstatus->str + sstatus->agi + sstatus->dex;
								break;
						}
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio += skillratio * s_level / 100;// Base level bonus.
					}
					break;
				case RL_MASS_SPIRAL:
					//Deals additional damage depending on targets DEF.
					//Skill desc says 200%*LV + Additional Damage from DEF
					//The additional damage is confirmed ice pick type.
					//Need a test and confirm on formula. [Rytech]
					skillratio = 200 * skill_lv;
					flag.pdef = flag.pdef2 = 1;
					break;
				case RL_BANISHING_BUSTER:
					skillratio = 1000 + 200 * skill_lv;
					break;
				case RL_H_MINE:
					skillratio = 200 + 200 * skill_lv;
					break;
				case RL_R_TRIP:
					skillratio = (10 + 3 * skill_lv) * sstatus->dex / 2;
					break;
				case RL_SLUGSHOT:
					if (sd)
					{
						//Ratio appears to be 10% for every 0.1 weight. Need more info. [Rytech]
						short index = sd->equip_index[EQI_AMMO];
						if( index >= 0 && sd->inventory_data[index] && sd->inventory_data[index]->type == IT_AMMO )
							skillratio = 10 * sd->inventory_data[index]->weight;
					}
					break;
				case KO_JYUMONJIKIRI:
					skillratio = 150 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 120;	// Base level bonus.
					{
						struct status_change *tsc = status_get_sc(target);
						if( tsc && tsc->data[SC_JYUMONJIKIRI] )// Bonus damage added when attacking target with Cross Wound status. [Rytech]
							if( re_baselv_bonus == 1 && s_level >= 100 )
								skillratio += skill_lv * s_level;	// Base level bonus.
							else
								skillratio += 150 * skill_lv;
					}
					break;
				case KO_SETSUDAN:
					skillratio = 100 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;	// Base level bonus.
					{
						struct status_change *tsc = status_get_sc(target);
						if( tsc && tsc->data[SC_SPIRIT] )// Bonus damage added when target is soul linked. [Rytech]
							skillratio += 200 * tsc->data[SC_SPIRIT]->val1;// Deals higher damage depending on level of soul link.
					}
					break;
				case KO_BAKURETSU:
					skillratio = (50 + sstatus->dex / 4) * skill_lv * pc_checkskill(sd, NJ_TOBIDOUGU) * 4 / 10;
					if( re_baselv_bonus == 1 && s_level >= 100 )
					{skillratio = skillratio * s_level / 120;	// Base level bonus.
					skillratio += 10 * status_get_job_lv(src);}
					else
					skillratio += 500;
					break;
				case KO_HUUMARANKA:
					skillratio = 150 * skill_lv + sstatus->agi + sstatus->dex + 100 * pc_checkskill(sd, NJ_HUUMA);
					break;
				case KO_MAKIBISHI:
					skillratio = 20 * skill_lv;
					break;
				case GC_DARKCROW:
					skillratio = 100 * skill_lv;
					break;
				case SU_BITE:
					skillratio = 200;
					break;
				case SU_SCRATCH:
					skillratio = 50 + 50 * skill_lv;
					break;
				case MH_NEEDLE_OF_PARALYZE:
					skillratio = 700 + 100 * skill_lv;
					break;
				case MH_SONIC_CRAW:
					skillratio = 40 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 150;
					break;
				case MH_SILVERVEIN_RUSH:
					skillratio = 150 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;
					break;
				case MH_MIDNIGHT_FRENZY:
					skillratio = 300 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 150;
					break;
				case MH_TINDER_BREAKER:
					skillratio = 100 * skill_lv + 3 * sstatus->str;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 120;
					break;
				case MH_STAHL_HORN:
					skillratio = 500 + 100 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = skillratio * s_level / 100;
					break;
				case MH_MAGMA_FLOW:
					skillratio = 100 * skill_lv;
					if( re_baselv_bonus == 1 && s_level >= 100 )
					{
						skillratio += 3 * s_level;
						skillratio = skillratio * s_level / 120;
					}
					else
						skillratio += 450;
					break;
				case MH_LAVA_SLIDE:
					if( re_baselv_bonus == 1 && s_level >= 100 )
					{
						skillratio = 20 * skill_lv + 2 * s_level;
						skillratio = skillratio * s_level / 100;
					}
					else
						skillratio = 20 * skill_lv + 300;
					break;
				// Physical Elemantal Spirits Attack Skills
				case EL_CIRCLE_OF_FIRE:
				case EL_FIRE_BOMB_ATK:
				case EL_STONE_RAIN:
					skillratio += 200;
					break;
				case EL_FIRE_WAVE_ATK:
					skillratio += 500;
					break;
				case EL_TIDAL_WEAPON:
					skillratio += 1400;
					break;
				case EL_WIND_SLASH:
					skillratio += 100;
					break;
				case EL_HURRICANE:
					skillratio += 600;
					break;
				case EL_TYPOON_MIS:
				case EL_WATER_SCREW_ATK:
					skillratio += 900;
					break;
				case EL_STONE_HAMMER:
					skillratio += 400;
					break;
				case EL_ROCK_CRUSHER:
					skillratio += 700;
					break;
			}

			ATK_RATE(skillratio);

			//Constant/misc additions from skills
			switch (skill_num) {
				case MO_EXTREMITYFIST:
					ATK_ADD(250 + 150*skill_lv);
					break;
				case TK_DOWNKICK:
				case TK_STORMKICK:
				case TK_TURNKICK:
				case TK_COUNTER:
				case TK_JUMPKICK:
					//TK_RUN kick damage bonus.
					if(sd && sd->weapontype1 == W_FIST && sd->weapontype2 == W_FIST)
						ATK_ADD(10*pc_checkskill(sd, TK_RUN));
					break;
				case GS_MAGICALBULLET:
					if(sstatus->matk_max>sstatus->matk_min) {
						ATK_ADD(sstatus->matk_min+rand()%(sstatus->matk_max-sstatus->matk_min));
					} else {
						ATK_ADD(sstatus->matk_min);
					}
					break;
				case NJ_SYURIKEN:
					ATK_ADD(4*skill_lv);
					break;
				case RA_WUGDASH:
					if(sd)
						ATK_ADD(sd->weight / 8);//Dont need to divide weight here since official formula takes current weight * 10. [Rytech]
					if( sc && sc->data[SC_DANCEWITHWUG] )
						ATK_ADD(2 * sc->data[SC_DANCEWITHWUG]->val1 * (2 + chorusbonus));
					break;
				case RA_WUGSTRIKE:
				case RA_WUGBITE:
					if(sd)
						ATK_ADD(30*pc_checkskill(sd, RA_TOOTHOFWUG));
					if( sc && sc->data[SC_DANCEWITHWUG] )
						ATK_ADD(2 * sc->data[SC_DANCEWITHWUG]->val1 * (2 + chorusbonus));
					break;
				case LG_SHIELDPRESS:
					if( sd )
					{
						int damagevalue = 0;
						short index = sd->equip_index[EQI_HAND_L];
						if( index >= 0 && sd->inventory_data[index] && sd->inventory_data[index]->type == IT_ARMOR )
							damagevalue = sstatus->vit * sd->status.inventory[index].refine;
						if( sc && sc->data[SC_GLOOMYDAY_SK] )
							damagevalue += damagevalue * (15 + rand()%sc->data[SC_GLOOMYDAY_SK]->val3) / 100;
						ATK_ADD(damagevalue);
					}
					break;
				//case LG_RAYOFGENESIS:
				//	if( sc && sc->data[SC_BANDING] )
				//	{// Increase only if the RG is under Banding.
				//		short lv = (short)skill_lv;
				//		ATK_ADDRATE( 190 * ((sd) ? skill_check_pc_partner(sd,(short)skill_num,&lv,skill_get_splash(skill_num,skill_lv),0) : 1));
				//	}
				//	break;
				case SR_FALLENEMPIRE:
					if ( tsd )
					{ATK_ADD(((tstatus->size + 1) * 2 + (skill_lv - 1)) * sstatus->str + tsd->weight / 10 * sstatus->dex / 120);}//For Player's
					else
					{ATK_ADD(((tstatus->size + 1) * 2 + (skill_lv - 1)) * sstatus->str + 50 * status_get_lv(target));}//For Monster's
					break;
				case SR_TIGERCANNON:
					if( sc && sc->data[SC_COMBO] )
					{ATK_ADD(500 * skill_lv + 40 * status_get_lv(target));}
					else
					{ATK_ADD(240 * skill_lv + 40 * status_get_lv(target));}
					break;
				case SR_GATEOFHELL:
					if( sc && sc->data[SC_COMBO] )
					{ATK_ADD((sstatus->max_hp - sstatus->hp) + sstatus->max_sp * ( 5 + skill_lv ) / 5 + 40 * s_level);}
					else
					{ATK_ADD((sstatus->max_hp - sstatus->hp) + sstatus->sp * ( 5 + skill_lv ) / 5 + 10 * s_level);}
					break;
				case MC_CARTREVOLUTION:
				case GN_CART_TORNADO:
				case GN_CARTCANNON:
					if( sc && sc->data[SC_GN_CARTBOOST] )
					ATK_ADD( 10 * sc->data[SC_GN_CARTBOOST]->val1 );
					break;
			}
		}
		//Div fix.
		damage_div_fix(wd.damage, wd.div_);

		//The following are applied on top of current damage and are stackable.
		if (sc) {
			if(sc->data[SC_TRUESIGHT])
				ATK_ADDRATE(2*sc->data[SC_TRUESIGHT]->val1);

			if(sc->data[SC_EDP] &&
			  	skill_num != ASC_BREAKER &&
				skill_num != ASC_METEORASSAULT &&
				skill_num != AS_SPLASHER &&
				skill_num != AS_VENOMKNIFE &&
				skill_num != GC_CROSSIMPACT &&//EDP does not increase the final damage on Guillotine Cross skills. It instead
				skill_num != GC_DARKILLUSION &&//increase the player's ATK (BATK and WATK) only during the brief moment certain
				skill_num != GC_COUNTERSLASH &&//offensive Guillotine Cross skills are being used to give a renewal feeling. [Rytech]
				skill_num != GC_WEAPONCRUSH &&
				skill_num != GC_VENOMPRESSURE &&
				skill_num != GC_PHANTOMMENACE &&
				skill_num != GC_ROLLINGCUTTER &&
				skill_num != GC_CROSSRIPPERSLASHER &&
				skill_num != GC_DARKCROW)//Unknown if Dark Claw is affected by EDP. Best to make it not until confirm. [Rytech]
				ATK_ADDRATE(sc->data[SC_EDP]->val3);

			// Heated Barrel increases damage of regular attacks.
			if(sc->data[SC_HEAT_BARREL] && (wd.flag&(BF_LONG|BF_WEAPON)) == (BF_LONG|BF_WEAPON) && (!skill_num))
				ATK_ADDRATE(sc->data[SC_HEAT_BARREL]->val3);

			if(sc->data[SC_UNLIMIT] && (wd.flag&(BF_LONG|BF_WEAPON)) == (BF_LONG|BF_WEAPON) &&
				(!skill_num ||//For regular attacks with bows.
				skill_num == AC_DOUBLE ||
				skill_num == AC_SHOWER ||
				skill_num == AC_CHARGEARROW ||
				skill_num == SN_SHARPSHOOTING ||
				skill_num == HT_POWER ||
				skill_num == HT_PHANTASMIC ||
				skill_num == RA_ARROWSTORM ||
				skill_num == RA_AIMEDBOLT ||
				skill_num == WM_SEVERE_RAINSTORM ||
				skill_num == WM_SEVERE_RAINSTORM_MELEE))
				ATK_ADDRATE(sc->data[SC_UNLIMIT]->val2);
		}

		switch (skill_num) {
			case AS_SONICBLOW:
				if (sc && sc->data[SC_SPIRIT] &&
					sc->data[SC_SPIRIT]->val2 == SL_ASSASIN)
					ATK_ADDRATE(map_flag_gvg(src->m)?25:100); //+25% dmg on woe/+100% dmg on nonwoe

				if(sd && pc_checkskill(sd,AS_SONICACCEL)>0)
					ATK_ADDRATE(10);
			break;
			case CR_SHIELDBOOMERANG:
				if(sc && sc->data[SC_SPIRIT] &&
					sc->data[SC_SPIRIT]->val2 == SL_CRUSADER)
					ATK_ADDRATE(100);
				break;
		}
		
		if( sd )
		{
			if (skill_num && (i = pc_skillatk_bonus(sd, skill_num)))
				ATK_ADDRATE(i);

			if( skill_num != PA_SACRIFICE && skill_num != MO_INVESTIGATE && skill_num != CR_GRANDCROSS && skill_num != NPC_GRANDDARKNESS && 
				skill_num != PA_SHIELDCHAIN && skill_num != NC_SELFDESTRUCTION && skill_num != KO_HAPPOKUNAI && skill_num != RL_MASS_SPIRAL && !flag.cri )
			{ //Elemental/Racial adjustments
				if( sd->right_weapon.def_ratio_atk_ele & (1<<tstatus->def_ele) ||
					sd->right_weapon.def_ratio_atk_race & (1<<tstatus->race) ||
					sd->right_weapon.def_ratio_atk_race & (1<<(is_boss(target)?RC_BOSS:RC_NONBOSS))
				)
					flag.pdef = 1;

				if( sd->left_weapon.def_ratio_atk_ele & (1<<tstatus->def_ele) ||
					sd->left_weapon.def_ratio_atk_race & (1<<tstatus->race) ||
					sd->left_weapon.def_ratio_atk_race & (1<<(is_boss(target)?RC_BOSS:RC_NONBOSS))
				)
				{ //Pass effect onto right hand if configured so. [Skotlex]
					if (battle_config.left_cardfix_to_right && flag.rh)
						flag.pdef = 1;
					else
						flag.pdef2 = 1;
				}
			}

			if (skill_num != CR_GRANDCROSS && skill_num != NPC_GRANDDARKNESS)
		  	{	//Ignore Defense?
				if (!flag.idef && (
					sd->right_weapon.ignore_def_ele & (1<<tstatus->def_ele) ||
					sd->right_weapon.ignore_def_race & (1<<tstatus->race) ||
					sd->right_weapon.ignore_def_race & (is_boss(target)?1<<RC_BOSS:1<<RC_NONBOSS)
				))
					flag.idef = 1;

				if (!flag.idef2 && (
					sd->left_weapon.ignore_def_ele & (1<<tstatus->def_ele) ||
					sd->left_weapon.ignore_def_race & (1<<tstatus->race) ||
					sd->left_weapon.ignore_def_race & (is_boss(target)?1<<RC_BOSS:1<<RC_NONBOSS)
				)) {
						if(battle_config.left_cardfix_to_right && flag.rh) //Move effect to right hand. [Skotlex]
							flag.idef = 1;
						else
							flag.idef2 = 1;
				}
			}
		}

		if (!flag.idef || !flag.idef2)
		{	//Defense reduction
			short vit_def;
			signed char def1 = status_get_def(target); //Don't use tstatus->def1 due to skill timer reductions.
			short def2 = (short)tstatus->def2;

			if( sc && sc->data[SC_EXPIATIO] )
			{ // Deffense bypass 5 * skilllv
				def1 -= def1 * 5 * sc->data[SC_EXPIATIO]->val1 / 100;
				def2 -= def2 * 5 * sc->data[SC_EXPIATIO]->val1 / 100;
			}

			if( sd )
			{
				i = sd->ignore_def[is_boss(target)?RC_BOSS:RC_NONBOSS];
				i += sd->ignore_def[tstatus->race];
				if( i )
				{
					if( i > 100 ) i = 100;
					def1 -= def1 * i / 100;
					def2 -= def2 * i / 100;
				}
			}

			if( battle_config.vit_penalty_type && battle_config.vit_penalty_target&target->type )
			{
				unsigned char target_count; //256 max targets should be a sane max
				target_count = unit_counttargeted(target,battle_config.vit_penalty_count_lv);
				if(target_count >= battle_config.vit_penalty_count) {
					if(battle_config.vit_penalty_type == 1) {
						if( !tsc || !tsc->data[SC_STEELBODY] )
							def1 = (def1 * (100 - (target_count - (battle_config.vit_penalty_count - 1))*battle_config.vit_penalty_num))/100;
						def2 = (def2 * (100 - (target_count - (battle_config.vit_penalty_count - 1))*battle_config.vit_penalty_num))/100;
					} else { //Assume type 2
						if( !tsc || !tsc->data[SC_STEELBODY] )
							def1 -= (target_count - (battle_config.vit_penalty_count - 1))*battle_config.vit_penalty_num;
						def2 -= (target_count - (battle_config.vit_penalty_count - 1))*battle_config.vit_penalty_num;
					}
				}
				if(skill_num == AM_ACIDTERROR) def1 = 0; //Acid Terror ignores only armor defense. [Skotlex]
				if(def2 < 1) def2 = 1;
			}
			//Vitality reduction from rodatazone: http://rodatazone.simgaming.net/mechanics/substats.php#def	
			if (tsd)	//Sd vit-eq
			{	//[VIT*0.5] + rnd([VIT*0.3], max([VIT*0.3],[VIT^2/150]-1))
				vit_def = def2*(def2-15)/150;
				vit_def = def2/2 + (vit_def>0?rand()%vit_def:0);
				
				if((battle_check_undead(sstatus->race,sstatus->def_ele) || sstatus->race==RC_DEMON) && //This bonus already doesnt work vs players
					src->type == BL_MOB && (skill=pc_checkskill(tsd,AL_DP)) > 0)
					vit_def += skill*(int)(3 +(tsd->status.base_level+1)*0.04);   // submitted by orn
				if( src->type == BL_MOB && (skill=pc_checkskill(tsd,RA_RANGERMAIN))>0 && 
					(sstatus->race == RC_BRUTE || sstatus->race == RC_FISH || sstatus->race == RC_PLANT) )
					vit_def += skill*5;
			} else { //Mob-Pet vit-eq
				//VIT + rnd(0,[VIT/20]^2-1)
				vit_def = (def2/20)*(def2/20);
				vit_def = def2 + (vit_def>0?rand()%vit_def:0);
			}
			
			if (battle_config.weapon_defense_type) {
				vit_def += def1*battle_config.weapon_defense_type;
				def1 = 0;
			}
			if (def1 > 100) def1 = 100;
			ATK_RATE2(
				flag.idef ?100:(flag.pdef ?(int)(flag.pdef *(def1+vit_def)):(100-def1)),
			 	flag.idef2?100:(flag.pdef2?(int)(flag.pdef2*(def1+vit_def)):(100-def1))
			);
			ATK_ADD2(
				flag.idef ||flag.pdef ?0:-vit_def,
				flag.idef2||flag.pdef2?0:-vit_def
			);
		}

		//Post skill/vit reduction damage increases
		if( sc && skill_num != LK_SPIRALPIERCE && skill_num != ML_SPIRALPIERCE )
		{	//SC skill damages
			if(sc->data[SC_AURABLADE]) 
				ATK_ADD(20*sc->data[SC_AURABLADE]->val1);
		}

		//Refine bonus
		if( sd && flag.weapon && skill_num != MO_INVESTIGATE && skill_num != MO_EXTREMITYFIST && skill_num != RL_MASS_SPIRAL )
		{ // Counts refine bonus multiple times
			if( skill_num == MO_FINGEROFFENSIVE )
			{
				ATK_ADD2(wd.div_*sstatus->rhw.atk2, wd.div_*sstatus->lhw.atk2);
			} else {
				ATK_ADD2(sstatus->rhw.atk2, sstatus->lhw.atk2);
			}
		}

		//Set to min of 1
		if (flag.rh && wd.damage < 1) wd.damage = 1;
		if (flag.lh && wd.damage2 < 1) wd.damage2 = 1;

		if (sd && flag.weapon &&
			skill_num != MO_INVESTIGATE &&
		  	skill_num != MO_EXTREMITYFIST &&
		  	skill_num != CR_GRANDCROSS &&
			skill_num != RL_MASS_SPIRAL)
		{	//Add mastery damage
			if(skill_num != ASC_BREAKER && sd->status.weapon == W_KATAR &&
				(skill=pc_checkskill(sd,ASC_KATAR)) > 0)
		  	{	//Adv Katar Mastery is does not applies to ASC_BREAKER,
				// but other masteries DO apply >_>
				ATK_ADDRATE(10+ 2*skill);
			}

			wd.damage = battle_addmastery(sd,target,wd.damage,0);
			if (flag.lh)
				wd.damage2 = battle_addmastery(sd,target,wd.damage2,1);

			if (sc && sc->data[SC_MIRACLE]) i = 2; //Star anger
			else
			ARR_FIND(0, MAX_PC_FEELHATE, i, t_class == sd->hate_mob[i]);
			if (i < MAX_PC_FEELHATE && (skill=pc_checkskill(sd,sg_info[i].anger_id))) 
			{
				skillratio = sd->status.base_level + sstatus->dex + sstatus->luk;
				if (i == 2) skillratio += sstatus->str; //Star Anger
				if (skill<4)
					skillratio /= 12-3*skill;
				ATK_ADDRATE(skillratio);
			}
			if (skill_num == NJ_SYURIKEN && (skill = pc_checkskill(sd,NJ_TOBIDOUGU)) > 0)
				ATK_ADD(3*skill);
			if (skill_num == NJ_KUNAI)
				ATK_ADD(60);
		}
	} //Here ends flag.hit section, the rest of the function applies to both hitting and missing attacks
  	else if(wd.div_ < 0) //Since the attack missed...
		wd.div_ *= -1; 

	if(sd && (skill=pc_checkskill(sd,BS_WEAPONRESEARCH)) > 0) 
		ATK_ADD(skill*2);

	if(skill_num==TF_POISON)
		ATK_ADD(15*skill_lv);

	if( !(nk&NK_NO_ELEFIX) && !n_ele )
	{	//Elemental attribute fix
		if( wd.damage > 0 )
		{
			wd.damage=battle_attr_fix(src,target,wd.damage,s_ele,tstatus->def_ele, tstatus->ele_lv);
			if( skill_num == MC_CARTREVOLUTION ) //Cart Revolution applies the element fix once more with neutral element
				wd.damage = battle_attr_fix(src,target,wd.damage,ELE_NEUTRAL,tstatus->def_ele, tstatus->ele_lv);
			if( skill_num== GS_GROUNDDRIFT ) //Additional 50*lv Neutral damage.
				wd.damage += battle_attr_fix(src,target,50*skill_lv,ELE_NEUTRAL,tstatus->def_ele, tstatus->ele_lv);
		}
		if( flag.lh && wd.damage2 > 0 )
			wd.damage2 = battle_attr_fix(src,target,wd.damage2,s_ele_,tstatus->def_ele, tstatus->ele_lv);
		if( sc && sc->data[SC_WATK_ELEMENT] )
		{ // Descriptions indicate this means adding a percent of a normal attack in another element. [Skotlex]
			int damage = battle_calc_base_damage(sstatus, &sstatus->rhw, sc, tstatus->size, sd, (flag.arrow?2:0)) * sc->data[SC_WATK_ELEMENT]->val2 / 100;
			wd.damage += battle_attr_fix(src, target, damage, sc->data[SC_WATK_ELEMENT]->val1, tstatus->def_ele, tstatus->ele_lv);

			if( flag.lh )
			{
				damage = battle_calc_base_damage(sstatus, &sstatus->lhw, sc, tstatus->size, sd, (flag.arrow?2:0)) * sc->data[SC_WATK_ELEMENT]->val2 / 100;
				wd.damage2 += battle_attr_fix(src, target, damage, sc->data[SC_WATK_ELEMENT]->val1, tstatus->def_ele, tstatus->ele_lv);
			}
		}
	}

	if(skill_num == CR_GRANDCROSS || skill_num == NPC_GRANDDARKNESS)
		return wd; //Enough, rest is not needed.

	if (sd)
	{
		if (skill_num != CR_SHIELDBOOMERANG) //Only Shield boomerang doesn't takes the Star Crumbs bonus.
			ATK_ADD2(wd.div_*sd->right_weapon.star, wd.div_*sd->left_weapon.star);
		if (skill_num==MO_FINGEROFFENSIVE) { //The finger offensive spheres on moment of attack do count. [Skotlex]
			ATK_ADD(wd.div_*sd->spiritball_old*3);
		} else {
			ATK_ADD(wd.div_*sd->spiritball*3);
		}

		//Card Fix, sd side
		if( (wd.damage || wd.damage2) && !(nk&NK_NO_CARDFIX_ATK) )
		{
			int cardfix = 1000, cardfix_ = 1000;
			int t_race2 = status_get_race2(target);
			if(sd->state.arrow_atk)
			{
				cardfix=cardfix*(100+sd->right_weapon.addrace[tstatus->race]+sd->arrow_addrace[tstatus->race])/100;
				if (!(nk&NK_NO_ELEFIX))
				{
					int ele_fix = sd->right_weapon.addele[tstatus->def_ele] + sd->arrow_addele[tstatus->def_ele];
					for (i = 0; ARRAYLENGTH(sd->right_weapon.addele2) > i && sd->right_weapon.addele2[i].rate != 0; i++) {
						if (sd->right_weapon.addele2[i].ele != tstatus->def_ele) continue;
						if(!(sd->right_weapon.addele2[i].flag&wd.flag&BF_WEAPONMASK &&
							 sd->right_weapon.addele2[i].flag&wd.flag&BF_RANGEMASK &&
							 sd->right_weapon.addele2[i].flag&wd.flag&BF_SKILLMASK))
								continue;
						ele_fix += sd->right_weapon.addele2[i].rate;
					}
					cardfix=cardfix*(100+ele_fix)/100;
				}
				cardfix=cardfix*(100+sd->right_weapon.addsize[tstatus->size]+sd->arrow_addsize[tstatus->size])/100;
				cardfix=cardfix*(100+sd->right_weapon.addrace2[t_race2])/100;
				cardfix=cardfix*(100+sd->right_weapon.addrace[is_boss(target)?RC_BOSS:RC_NONBOSS]+sd->arrow_addrace[is_boss(target)?RC_BOSS:RC_NONBOSS])/100;
				if( tstatus->race != RC_DEMIHUMAN )
					cardfix=cardfix*(100+sd->right_weapon.addrace[RC_NONDEMIHUMAN]+sd->arrow_addrace[RC_NONDEMIHUMAN])/100;
			}
			else
			{ // Melee attack
				if( !battle_config.left_cardfix_to_right )
				{
					cardfix=cardfix*(100+sd->right_weapon.addrace[tstatus->race])/100;					
					if (!(nk&NK_NO_ELEFIX)) {
						int ele_fix = sd->right_weapon.addele[tstatus->def_ele];
						for (i = 0; ARRAYLENGTH(sd->right_weapon.addele2) > i && sd->right_weapon.addele2[i].rate != 0; i++) {
							if (sd->right_weapon.addele2[i].ele != tstatus->def_ele) continue;
							if(!(sd->right_weapon.addele2[i].flag&wd.flag&BF_WEAPONMASK &&
								 sd->right_weapon.addele2[i].flag&wd.flag&BF_RANGEMASK &&
								 sd->right_weapon.addele2[i].flag&wd.flag&BF_SKILLMASK))
									continue;
							ele_fix += sd->right_weapon.addele2[i].rate;
						}
						cardfix=cardfix*(100+ele_fix)/100;
					}
					cardfix=cardfix*(100+sd->right_weapon.addsize[tstatus->size])/100;
					cardfix=cardfix*(100+sd->right_weapon.addrace2[t_race2])/100;
					cardfix=cardfix*(100+sd->right_weapon.addrace[is_boss(target)?RC_BOSS:RC_NONBOSS])/100;
					if( tstatus->race != RC_DEMIHUMAN )
						cardfix=cardfix*(100+sd->right_weapon.addrace[RC_NONDEMIHUMAN])/100;

					if( flag.lh )
					{
						cardfix_=cardfix_*(100+sd->left_weapon.addrace[tstatus->race])/100;						
						if (!(nk&NK_NO_ELEFIX))	{
							int ele_fix_lh = sd->left_weapon.addele[tstatus->def_ele];							
							for (i = 0; ARRAYLENGTH(sd->left_weapon.addele2) > i && sd->left_weapon.addele2[i].rate != 0; i++) {
								if (sd->left_weapon.addele2[i].ele != tstatus->def_ele) continue;
								if(!(sd->left_weapon.addele2[i].flag&wd.flag&BF_WEAPONMASK &&
									 sd->left_weapon.addele2[i].flag&wd.flag&BF_RANGEMASK &&
									 sd->left_weapon.addele2[i].flag&wd.flag&BF_SKILLMASK))
										continue;
								ele_fix_lh += sd->left_weapon.addele2[i].rate;
							}
							cardfix=cardfix*(100+ele_fix_lh)/100;
						}
						cardfix_=cardfix_*(100+sd->left_weapon.addsize[tstatus->size])/100;
						cardfix_=cardfix_*(100+sd->left_weapon.addrace2[t_race2])/100;
						cardfix_=cardfix_*(100+sd->left_weapon.addrace[is_boss(target)?RC_BOSS:RC_NONBOSS])/100;
						if( tstatus->race != RC_DEMIHUMAN )
							cardfix_=cardfix_*(100+sd->left_weapon.addrace[RC_NONDEMIHUMAN])/100;
					}
				}
				else
				{
					int ele_fix = sd->right_weapon.addele[tstatus->def_ele] + sd->left_weapon.addele[tstatus->def_ele];
					for (i = 0; ARRAYLENGTH(sd->right_weapon.addele2) > i && sd->right_weapon.addele2[i].rate != 0; i++) {
						if (sd->right_weapon.addele2[i].ele != tstatus->def_ele) continue;
						if(!(sd->right_weapon.addele2[i].flag&wd.flag&BF_WEAPONMASK &&
							 sd->right_weapon.addele2[i].flag&wd.flag&BF_RANGEMASK &&
							 sd->right_weapon.addele2[i].flag&wd.flag&BF_SKILLMASK))
								continue;
						ele_fix += sd->right_weapon.addele2[i].rate;
					}
					for (i = 0; ARRAYLENGTH(sd->left_weapon.addele2) > i && sd->left_weapon.addele2[i].rate != 0; i++) {
						if (sd->left_weapon.addele2[i].ele != tstatus->def_ele) continue;
						if(!(sd->left_weapon.addele2[i].flag&wd.flag&BF_WEAPONMASK &&
							 sd->left_weapon.addele2[i].flag&wd.flag&BF_RANGEMASK &&
							 sd->left_weapon.addele2[i].flag&wd.flag&BF_SKILLMASK))
								continue;
						ele_fix += sd->left_weapon.addele2[i].rate;
					}

					cardfix=cardfix*(100+sd->right_weapon.addrace[tstatus->race]+sd->left_weapon.addrace[tstatus->race])/100;
					cardfix=cardfix*(100+ele_fix)/100;
					cardfix=cardfix*(100+sd->right_weapon.addsize[tstatus->size]+sd->left_weapon.addsize[tstatus->size])/100;
					cardfix=cardfix*(100+sd->right_weapon.addrace2[t_race2]+sd->left_weapon.addrace2[t_race2])/100;
					cardfix=cardfix*(100+sd->right_weapon.addrace[is_boss(target)?RC_BOSS:RC_NONBOSS]+sd->left_weapon.addrace[is_boss(target)?RC_BOSS:RC_NONBOSS])/100;
					if( tstatus->race != RC_DEMIHUMAN )
						cardfix=cardfix*(100+sd->right_weapon.addrace[RC_NONDEMIHUMAN]+sd->left_weapon.addrace[RC_NONDEMIHUMAN])/100;
				}
			}

			for( i = 0; i < ARRAYLENGTH(sd->right_weapon.add_dmg) && sd->right_weapon.add_dmg[i].rate; i++ )
			{
				if( sd->right_weapon.add_dmg[i].class_ == t_class )
				{
					cardfix=cardfix*(100+sd->right_weapon.add_dmg[i].rate)/100;
					break;
				}
			}

			if( flag.lh )
			{
				for( i = 0; i < ARRAYLENGTH(sd->left_weapon.add_dmg) && sd->left_weapon.add_dmg[i].rate; i++ )
				{
					if( sd->left_weapon.add_dmg[i].class_ == t_class )
					{
						cardfix_=cardfix_*(100+sd->left_weapon.add_dmg[i].rate)/100;
						break;
					}
				}
			}

			if( wd.flag&BF_LONG )
				cardfix=cardfix*(100+sd->long_attack_atk_rate)/100;

			if( cardfix != 1000 || cardfix_ != 1000 )
				ATK_RATE2(cardfix/10, cardfix_/10);	//What happens if you use right-to-left and there's no right weapon, only left?
		}
		
		if( skill_num == CR_SHIELDBOOMERANG || skill_num == PA_SHIELDCHAIN )
		{ //Refine bonus applies after cards and elements.
			int damagevalue = 0;
			short index= sd->equip_index[EQI_HAND_L];
			if( index >= 0 && sd->inventory_data[index] && sd->inventory_data[index]->type == IT_ARMOR )
				damagevalue = 10 * sd->status.inventory[index].refine;
			if( sc && sc->data[SC_GLOOMYDAY_SK] )//????
				damagevalue += damagevalue * (15 + rand()%sc->data[SC_GLOOMYDAY_SK]->val3) / 100;
			ATK_ADD(damagevalue);
		}
	} //if (sd)
	else if (hd)
	{// Homunculus Spirit Sphere's ATK Bonus
		ATK_ADD(wd.div_*hd->hom_spiritball*3);
	}

	//Card Fix, tsd sid
	if( tsd && !(nk&NK_NO_CARDFIX_DEF) )
	{
		short s_race2,s_class;
		short cardfix=1000;

		s_race2 = status_get_race2(src);
		s_class = status_get_class(src);

		if( !(nk&NK_NO_ELEFIX) )
		{
			int ele_fix = tsd->subele[s_ele];
			for (i = 0; ARRAYLENGTH(tsd->subele2) > i && tsd->subele2[i].rate != 0; i++)
			{
				if(tsd->subele2[i].ele != s_ele) continue;
				if(!(tsd->subele2[i].flag&wd.flag&BF_WEAPONMASK &&
					 tsd->subele2[i].flag&wd.flag&BF_RANGEMASK &&
					 tsd->subele2[i].flag&wd.flag&BF_SKILLMASK))
					continue;
				ele_fix += tsd->subele2[i].rate;
			}
			cardfix=cardfix*(100-ele_fix)/100;
			if( flag.lh && s_ele_ != s_ele )
			{
				int ele_fix_lh = tsd->subele[s_ele_];
				for (i = 0; ARRAYLENGTH(tsd->subele2) > i && tsd->subele2[i].rate != 0; i++)
				{
					if(tsd->subele2[i].ele != s_ele_) continue;
					if(!(tsd->subele2[i].flag&wd.flag&BF_WEAPONMASK &&
						 tsd->subele2[i].flag&wd.flag&BF_RANGEMASK &&
						 tsd->subele2[i].flag&wd.flag&BF_SKILLMASK))
						continue;
					ele_fix_lh += tsd->subele2[i].rate;
				}
				cardfix=cardfix*(100-ele_fix_lh)/100;
			}
		}
		cardfix=cardfix*(100-tsd->subsize[sstatus->size])/100;
 		cardfix=cardfix*(100-tsd->subrace2[s_race2])/100;
		cardfix=cardfix*(100-tsd->subrace[sstatus->race])/100;
		cardfix=cardfix*(100-tsd->subrace[is_boss(src)?RC_BOSS:RC_NONBOSS])/100;
		if( sstatus->race != RC_DEMIHUMAN )
			cardfix=cardfix*(100-tsd->subrace[RC_NONDEMIHUMAN])/100;

		for( i = 0; i < ARRAYLENGTH(tsd->add_def) && tsd->add_def[i].rate;i++ )
		{
			if( tsd->add_def[i].class_ == s_class )
			{
				cardfix=cardfix*(100-tsd->add_def[i].rate)/100;
				break;
			}
		}

		if( wd.flag&BF_SHORT )
			cardfix=cardfix*(100-tsd->near_attack_def_rate)/100;
		else	// BF_LONG (there's no other choice)
			cardfix=cardfix*(100-tsd->long_attack_def_rate)/100;

		if( tsd->sc.data[SC_DEF_RATE] )
			cardfix=cardfix*(100-tsd->sc.data[SC_DEF_RATE]->val1)/100;

		if( cardfix != 1000 )
			ATK_RATE(cardfix/10);
	}

	if( flag.infdef )
	{ //Plants receive 1 damage when hit
		if( flag.hit || wd.damage > 0 )
			wd.damage = wd.div_; // In some cases, right hand no need to have a weapon to increase damage
		if( flag.lh && (flag.hit || wd.damage2 > 0) )
			wd.damage2 = wd.div_;
		if( !(battle_config.skill_min_damage&1) )
			//Do not return if you are supposed to deal greater damage to plants than 1. [Skotlex]
			return wd;
	}

	if( sd )
	{
		if( !flag.rh && flag.lh )
		{	//Move lh damage to the rh
			wd.damage = wd.damage2;
			wd.damage2 = 0;
			flag.rh = 1;
			flag.lh = 0;
		}
		else if( flag.rh && flag.lh )
		{	//Dual-wield
			if (wd.damage)
			{
				if ((sd->class_&MAPID_UPPERMASK) == MAPID_KAGEROUOBORO)
				{skill = pc_checkskill(sd,KO_RIGHT);
				wd.damage = wd.damage * (70 + (skill * 10)) / 100;}
				else
				{skill = pc_checkskill(sd,AS_RIGHT);
				wd.damage = wd.damage * (50 + (skill * 10))/100;}
				if(wd.damage < 1)
					wd.damage = 1;
			}
			if (wd.damage2)
			{
				if ((sd->class_&MAPID_UPPERMASK) == MAPID_KAGEROUOBORO)
				{skill = pc_checkskill(sd,KO_LEFT);
				wd.damage2 = wd.damage2 * (50 + (skill * 10)) / 100;}
				else
				{skill = pc_checkskill(sd,AS_LEFT);
				wd.damage2 = wd.damage2 * (30 + (skill * 10))/100;}
				if(wd.damage2 < 1)
					wd.damage2 = 1;
			}
		}
		else if( sd->status.weapon == W_KATAR && !skill_num )
		{ //Katars (offhand damage only applies to normal attacks, tested on Aegis 10.2)
			skill = pc_checkskill(sd,TF_DOUBLE);
			wd.damage2 = wd.damage * (1 + (skill * 2)) / 100;

			if( wd.damage && !wd.damage2 )
				wd.damage2 = 1;

			flag.lh = 1;
		}
	}

	if( !flag.rh && wd.damage )
		wd.damage = 0;

	if( !flag.lh && wd.damage2 )
		wd.damage2 = 0;

	if( wd.damage + wd.damage2 )
	{	//There is a total damage value
		if( wd.damage )
		{
			wd.damage = battle_calc_damage(src,target,&wd,wd.damage,skill_num,skill_lv,s_ele);
			if( map_flag_gvg2(target->m) )
				wd.damage = battle_calc_gvg_damage(src,target,wd.damage,wd.div_,skill_num,skill_lv,wd.flag);
			else if( map[target->m].flag.battleground )
				wd.damage = battle_calc_bg_damage(src,target,wd.damage,wd.div_,skill_num,skill_lv,wd.flag);
		}
		if( wd.damage2 )
		{
			wd.damage2 = battle_calc_damage(src,target,&wd,wd.damage2,skill_num,skill_lv,s_ele_);
			if( map_flag_gvg2(target->m) )
				wd.damage2 = battle_calc_gvg_damage(src,target,wd.damage2,wd.div_,skill_num,skill_lv,wd.flag);
			else if( map[target->m].flag.battleground )
				wd.damage = battle_calc_bg_damage(src,target,wd.damage2,wd.div_,skill_num,skill_lv,wd.flag);
		}
	}

	if(skill_num==ASC_BREAKER)
	{	//Breaker's int-based damage (a misc attack?)
		struct Damage md = battle_calc_misc_attack(src, target, skill_num, skill_lv, wflag);
		wd.damage += md.damage;
	}

	//SG_FUSION hp penalty [Komurka]
	if( sc && sc->data[SC_FUSION] )
	{
		int hp = sstatus->max_hp;
		if( sd && tsd )
		{
			hp = 8 * hp / 100;
			if( 100 * sstatus->hp <= 20 * sstatus->max_hp )
				hp = sstatus->hp;
		}
		else
			hp = 2 * hp / 100; //2% hp loss per hit
		status_zap(src, hp, 0);
	}

	if( sc && sc->data[SC_ENCHANTBLADE] && !skill_num && sd && ((flag.rh && sd->weapontype1) || (flag.lh && sd->weapontype2)) ) // Only regular melee attacks are increased. A weapon must be equiped. [pakpil]
	{	// Magic damage from Enchant Blade
		struct Damage md = battle_calc_magic_attack(src, target, RK_ENCHANTBLADE, ((TBL_PC*)src)->status.skill[RK_ENCHANTBLADE].lv, wflag);
		wd.damage += md.damage;
		wd.flag |= md.flag;
	}
	if( skill_num == LG_RAYOFGENESIS )
	{
		struct Damage md = battle_calc_magic_attack(src, target, skill_num, skill_lv, wflag);
		wd.damage += md.damage;
	}

	return wd;
}

/*==========================================
 * battle_calc_magic_attack [DracoRPG]
 *------------------------------------------*/
struct Damage battle_calc_magic_attack(struct block_list *src,struct block_list *target,int skill_num,int skill_lv,int mflag)
{
	int i, nk, s_level, s_job_level = 50;
	short s_ele = skill_get_ele(skill_num, skill_lv);
	unsigned int skillratio = 100;	//Skill dmg modifiers.
	char re_baselv_bonus = battle_config.renewal_baselvl_skill_ratio;//BaseLv config for renewal skills.

	struct map_session_data *sd, *tsd;
	struct Damage ad;
	struct status_change *sc = status_get_sc(src);
	struct status_change *tsc = status_get_sc(target);
	struct status_data *sstatus = status_get_status_data(src);
	struct status_data *tstatus = status_get_status_data(target);

	struct
	{
		unsigned imdef : 1;
		unsigned infdef : 1;
	}flag;

	memset(&ad,0,sizeof(ad));
	memset(&flag,0,sizeof(flag));

	if( src == NULL || target == NULL )
	{
		nullpo_info(NLP_MARK);
		return ad;
	}
	sd = BL_CAST(BL_PC, src);
	tsd = BL_CAST(BL_PC, target);

	s_level = status_get_lv(src);
	if( skill_num >= RK_ENCHANTBLADE && skill_num <= LG_OVERBRAND_PLUSATK &&
		battle_config.max_highlvl_nerf && s_level > battle_config.max_highlvl_nerf )
		s_level = battle_config.max_highlvl_nerf;
		
	// Max Job Level bonus that skills should receive. Acording to battle_config.max_joblvl_nerf [Pinky]
	if( sd && battle_config.max_joblvl_nerf)
		s_job_level = min(sd->status.job_level,battle_config.max_joblvl_nerf);
	else if ( sd )
		s_job_level = sd->status.job_level;

	//Initial Values
	ad.damage = 1;
	ad.amotion=skill_get_inf(skill_num)&INF_GROUND_SKILL?0:sstatus->amotion; //Amotion should be 0 for ground skills.
	ad.dmotion=tstatus->dmotion;
	ad.flag=BF_MAGIC|BF_SKILL;
	ad.dmg_lv=ATK_DEF;
	nk = skill_get_nk(skill_num);
	flag.imdef = nk&NK_IGNORE_DEF?1:0;

	// Skill Element Definition// I dont understand why were checking for skill level here. [Rytech]
	if( skill_num == WL_HELLINFERNO )
	{
		if( skill_lv >= 0 )
			s_ele = ELE_FIRE;
		else
		{
			s_ele = ELE_DARK;
			skill_lv = -skill_lv;
		}
	}
	else if ( skill_num == LG_HESPERUSLIT && sc && sc->data[SC_BANDING] && sc->data[SC_BANDING]->val2 > 4 )
	{
			s_ele = ELE_HOLY;
	}
	else if( sc && (sc->data[SC_HEATER_OPTION] || sc->data[SC_COOLER_OPTION] ||
		sc->data[SC_BLAST_OPTION] || sc->data[SC_CURSED_SOIL_OPTION]) && skill_num == SO_PSYCHIC_WAVE )
	{	// Status change from Elemental Spirits that change Psychic Wave damage element.
		if( sc->data[SC_HEATER_OPTION] )
			s_ele = sc->data[SC_HEATER_OPTION]->val4;
		else if( sc->data[SC_COOLER_OPTION] )
			s_ele = sc->data[SC_COOLER_OPTION]->val4;
		else if( sc->data[SC_BLAST_OPTION] )
			s_ele = sc->data[SC_BLAST_OPTION]->val3;
		else if( sc->data[SC_CURSED_SOIL_OPTION] )
			s_ele = sc->data[SC_CURSED_SOIL_OPTION]->val4;
	}
	else if ( skill_num == KO_KAIHOU )
	{
		if ( sc->data[SC_KAHU_ENTEN] )
			s_ele = ELE_FIRE;
		else if ( sc->data[SC_HYOUHU_HUBUKI] )
			s_ele = ELE_WATER;
		else if ( sc->data[SC_KAZEHU_SEIRAN] )
			s_ele = ELE_WIND;
		else if ( sc->data[SC_DOHU_KOUKAI] )
			s_ele = ELE_EARTH;
		else//If a GM character trys to use the skill with no sphere's, one of the four status will likely not be active.
			s_ele = ELE_NEUTRAL;//If so, then use neutral element. [Rytech]
	}
	else
	{
		s_ele = skill_get_ele(skill_num, skill_lv);
		if( s_ele == -1 )
			s_ele = sstatus->rhw.ele;
		else if( s_ele == -2 )
			s_ele = status_get_attack_sc_element(src,sc);
		else if( s_ele == -3 ) //Use random element
			s_ele = rand()%ELE_MAX;
	}

	ad.div_=skill_get_num(skill_num,skill_lv);
	ad.blewcount = skill_get_blewcount(skill_num,skill_lv);

	//Set miscellaneous data that needs be filled
	if(sd) {
		sd->state.arrow_atk = 0;
		ad.blewcount += battle_blewcount_bonus(sd, skill_num);
	}

	//Skill Range Criteria
	ad.flag |= battle_range_type(src, target, skill_num, skill_lv);
	flag.infdef = (tstatus->mode&MD_PLANT?1:0);

	if( target->type == BL_SKILL && ((TBL_SKILL*)target)->group->unit_id == UNT_REVERBERATION )
		flag.infdef = 1; // Reberberation takes 1 damage

	switch(skill_num)
	{
		case MG_FIREWALL:
		case NJ_KAENSIN:
		case EL_FIRE_MANTLE:
			ad.dmotion = 0; //No flinch animation.
			if ( tstatus->def_ele == ELE_FIRE || battle_check_undead(tstatus->race, tstatus->def_ele) )
				ad.blewcount = 0; //No knockback
			break;
		case PR_SANCTUARY:
			ad.dmotion = 0; //No flinch animation.
			break;
	}

	if (!flag.infdef) //No need to do the math for plants
	{

//MATK_RATE scales the damage. 100 = no change. 50 is halved, 200 is doubled, etc
#define MATK_RATE( a ) { ad.damage= ad.damage*(a)/100; }
//Adds dmg%. 100 = +100% (double) damage. 10 = +10% damage
#define MATK_ADDRATE( a ) { ad.damage+= ad.damage*(a)/100; }
//Adds an absolute value to damage. 100 = +100 damage
#define MATK_ADD( a ) { ad.damage+= a; }

		switch (skill_num)
		{	//Calc base damage according to skill
			case AL_HEAL:
			case PR_BENEDICTIO:
			case PR_SANCTUARY:
			case AB_HIGHNESSHEAL:
				ad.damage = skill_calc_heal(src, target, skill_num, skill_lv, false);
				break;
			case PR_ASPERSIO:
				ad.damage = 40;
				break;
			case ALL_RESURRECTION:
			case PR_TURNUNDEAD:
				//Undead check is on skill_castend_damageid code.
				i = 20*skill_lv + sstatus->luk + sstatus->int_ + s_level
				  	+ 200 - 200*tstatus->hp/tstatus->max_hp;
				if(i > 700) i = 700;
				if(rand()%1000 < i && !(tstatus->mode&MD_BOSS))
					ad.damage = tstatus->hp;
				else
					ad.damage = s_level + sstatus->int_ + skill_lv * 10;
				break;
			case PF_SOULBURN:
				ad.damage = tstatus->sp * 2;
				break;
			/* Enable this if kRO fix the current skill. Currently no damage to undead or demons. [Jobbie]
			case AB_EPICLESIS:
				ad.damage = ??;
				break;*/
			case AB_RENOVATIO://Changed to a official formula, but this damage will later be applied differently through a duration. [Rytech]
				ad.damage = 10 * s_level + sstatus->int_;
				break;
			default:
			{
				if( skill_num == RK_ENCHANTBLADE )
				{
					if( sc && sc->data[SC_ENCHANTBLADE] )
						ad.damage += sc->data[SC_ENCHANTBLADE]->val2;
					else
						return ad;
				}
				if(sc && sc->data[SC_RECOGNIZEDSPELL]) {
					MATK_ADD(sstatus->matk_max);
				}
				else if (sstatus->matk_max > sstatus->matk_min) {
					MATK_ADD(sstatus->matk_min+rand()%(1+sstatus->matk_max-sstatus->matk_min));
				} else {
					MATK_ADD(sstatus->matk_min);
				}

				if(nk&NK_SPLASHSPLIT){ // Divide MATK in case of multiple targets skill
					if(mflag>0)
						ad.damage/= mflag;
					else
						ShowError("0 enemies targeted by %d:%s, divide per 0 avoided!\n", skill_num, skill_get_name(skill_num));
				}

				switch( skill_num )
				{
					case MG_NAPALMBEAT:
					case MG_FIREBALL:
						skillratio += skill_lv*10-30;
						break;
					case MG_SOULSTRIKE:
						if (battle_check_undead(tstatus->race,tstatus->def_ele))
							skillratio += 5*skill_lv;
						break;
					case MG_COLDBOLT:
						if ( sc )
						{
							if ( sc->data[SC_SPELLFIST] && (!sd || !sd->state.autocast))
							{
								skillratio = sc->data[SC_SPELLFIST]->val2 * 50 + sc->data[SC_SPELLFIST]->val4 * 100;//val2 = used spellfist level and val4 = used bolt level. [Rytech]
								ad.div_ = 1;// ad mods, to make it work similar to regular hits [Xazax]
								ad.flag = BF_WEAPON|BF_SHORT;
								ad.type = 0;
							}
							if( sc->data[SC_AQUAPLAY_OPTION] )
								skillratio += skillratio * sc->data[SC_AQUAPLAY_OPTION]->val3 / 100;
						}
						break;
					case MG_FIREWALL:
						skillratio -= 50;
						if( sc && sc->data[SC_PYROTECHNIC_OPTION] )
							skillratio += skillratio * sc->data[SC_PYROTECHNIC_OPTION]->val3 / 100;
						break;
					case MG_FIREBOLT:
						if ( sc )
						{
							if ( sc->data[SC_SPELLFIST] && (!sd || !sd->state.autocast))
							{
								skillratio = sc->data[SC_SPELLFIST]->val2 * 50 + sc->data[SC_SPELLFIST]->val4 * 100;
								ad.div_ = 1;
								ad.flag = BF_WEAPON|BF_SHORT;
								ad.type = 0;
							}
							if( sc->data[SC_PYROTECHNIC_OPTION] )
								skillratio += skillratio * sc->data[SC_PYROTECHNIC_OPTION]->val3 / 100;
						}
						break;
					case MG_LIGHTNINGBOLT:
						if ( sc )
						{
							if ( sc->data[SC_SPELLFIST] && (!sd || !sd->state.autocast))
							{
								skillratio = sc->data[SC_SPELLFIST]->val2 * 50 + sc->data[SC_SPELLFIST]->val4 * 100;
								ad.div_ = 1;
								ad.flag = BF_WEAPON|BF_SHORT;
								ad.type = 0;
							}
							if( sc->data[SC_GUST_OPTION] )
								skillratio += skillratio * sc->data[SC_GUST_OPTION]->val2 / 100;
						}
						break;
					case MG_THUNDERSTORM:
						skillratio -= 20;
						if( sc && sc->data[SC_GUST_OPTION] )
							skillratio += skillratio * sc->data[SC_GUST_OPTION]->val2 / 100;
						break;
					case MG_FROSTDIVER:
						skillratio += 10*skill_lv;
						if( sc && sc->data[SC_AQUAPLAY_OPTION] )
							skillratio += skillratio * sc->data[SC_AQUAPLAY_OPTION]->val3 / 100;
						break;
					case AL_HOLYLIGHT:
						skillratio += 25;
						if (sd && sd->sc.data[SC_SPIRIT] && sd->sc.data[SC_SPIRIT]->val2 == SL_PRIEST)
							skillratio *= 5; //Does 5x damage include bonuses from other skills?
						break;
					case AL_RUWACH:
						skillratio += 45;
						break;
					case WZ_FROSTNOVA:
						skillratio += (100+skill_lv*10)*2/3-100;
						break;
					case WZ_FIREPILLAR:
						if (skill_lv > 10)
							skillratio += 100;
						else
							skillratio -= 80;
						break;
					case WZ_SIGHTRASHER:
						skillratio += 20*skill_lv;
						break;
					case WZ_VERMILION:
						skillratio += 20*skill_lv-20;
						break;
					case WZ_WATERBALL:
						skillratio += 30*skill_lv;
						break;
					case WZ_EARTHSPIKE:
					case WZ_HEAVENDRIVE:
						if( sc && sc->data[SC_PETROLOGY_OPTION] )
							skillratio += skillratio * sc->data[SC_PETROLOGY_OPTION]->val3 / 100;
						break;
					case WZ_STORMGUST:
						skillratio += 40*skill_lv;
						break;
					case HW_NAPALMVULCAN:
						skillratio += 10*skill_lv-30;
						break;
					case SL_STIN:
						skillratio += (tstatus->size?-99:10*skill_lv); //target size must be small (0) for full damage.
						break;
					case SL_STUN:
						skillratio += (tstatus->size!=2?5*skill_lv:-99); //Full damage is dealt on small/medium targets
						break;
					case SL_SMA:
						skillratio += -60 + s_level; //Base damage is 40% + lv%
						break;
					case NJ_KOUENKA:
						skillratio -= 10;
						if( sc && sc->data[SC_KAHU_ENTEN] )
							skillratio += 20 * sc->data[SC_KAHU_ENTEN]->val2;
						break;
					case NJ_KAENSIN:
						skillratio -= 50;
						if( sc && sc->data[SC_KAHU_ENTEN] )
							skillratio += 5 * sc->data[SC_KAHU_ENTEN]->val2;
						break;
					case NJ_BAKUENRYU:
						skillratio += 50*(skill_lv-1);
						if( sc && sc->data[SC_KAHU_ENTEN] )
							skillratio += 15 * sc->data[SC_KAHU_ENTEN]->val2;
						break;
					case NJ_HYOUSENSOU:
						if( sc && sc->data[SC_HYOUHU_HUBUKI] )
							skillratio += 5 * sc->data[SC_HYOUHU_HUBUKI]->val2;
						break;
					case NJ_HYOUSYOURAKU:
						skillratio += 50*skill_lv;
						if( sc && sc->data[SC_HYOUHU_HUBUKI] )
							skillratio += 25 * sc->data[SC_HYOUHU_HUBUKI]->val2;
						break;
					case NJ_HUUJIN:
						if( sc && sc->data[SC_KAZEHU_SEIRAN] )
							skillratio += 20 * sc->data[SC_KAZEHU_SEIRAN]->val2;
						break;
					case NJ_RAIGEKISAI:
						skillratio += 60 + 40*skill_lv;
						if( sc && sc->data[SC_KAZEHU_SEIRAN] )
							skillratio += 15 * sc->data[SC_KAZEHU_SEIRAN]->val2;
						break;
					case NJ_KAMAITACHI:
						skillratio += 100*skill_lv;
						if( sc && sc->data[SC_KAZEHU_SEIRAN] )
							skillratio += 10 * sc->data[SC_KAZEHU_SEIRAN]->val2;
						break;
					case NPC_ENERGYDRAIN:
						skillratio += 100*skill_lv;
						break;
					case NPC_EARTHQUAKE:
						skillratio += 100 + 100*skill_lv + 100*(skill_lv/2);
						break;
					case AB_JUDEX:
						skillratio = 300 + 20 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						break;
					case AB_ADORAMUS:
						skillratio += 400 + 100 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						break;
					case AB_DUPLELIGHT_MAGIC:
						skillratio += 100 + 20 * skill_lv;
						break;
					case WL_SOULEXPANSION:
						skillratio += 300 + 100 * skill_lv + sstatus->int_;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						break;
					case WL_FROSTMISTY:
						skillratio += 100 + 100 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						break;
					case WL_JACKFROST:
						if( tsc && tsc->data[SC_FREEZING] )
						{
							skillratio += 900 + 300 * skill_lv;
							if( re_baselv_bonus == 1 && s_level >= 100 )
								skillratio = skillratio * s_level / 100;	// Base level bonus.
						}
						else
						{
							skillratio += 400 + 100 * skill_lv;
							if( re_baselv_bonus == 1 && s_level >= 100 )
								skillratio = skillratio * s_level / 150;	// Base level bonus.
						}
						break;
					case WL_DRAINLIFE:
						skillratio = 200 * skill_lv + sstatus->int_;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						break;
					case WL_CRIMSONROCK:
						skillratio += 1200 + 300 * skill_lv;//2011 document says the 1300+ is applied after the base level bonus. Must confirm. [Rytech]
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						break;
					case WL_HELLINFERNO:
						if( s_ele == ELE_FIRE )
							skillratio = 60 * skill_lv;
						else
							skillratio = 240 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						break;
					case WL_COMET:
						i = distance_xy(target->x, target->y, sc->comet_x, sc->comet_y);
						if( i < 4 ) skillratio = 2500 + 500 * skill_lv;
						else
						if( i < 6 ) skillratio = 2000 + 500 * skill_lv;
						else
						if( i < 8 ) skillratio = 1500 + 500 * skill_lv;
						else
						skillratio = 1000 + 500 * skill_lv;
						break;
					case WL_CHAINLIGHTNING_ATK:
						skillratio += 400 + 100 * skill_lv + 500;//Will replace the 500 with the bounce part to the formula soon. [Rytech]
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						break;
					case WL_EARTHSTRAIN:
						skillratio += 1900 + 100 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						break;
					case WL_TETRAVORTEX_FIRE:
					case WL_TETRAVORTEX_WATER:
					case WL_TETRAVORTEX_WIND:
					case WL_TETRAVORTEX_GROUND:
						skillratio += 400 + 500 * skill_lv;
						break;
					case WL_SUMMON_ATK_FIRE:
					case WL_SUMMON_ATK_WATER:
					case WL_SUMMON_ATK_WIND:
					case WL_SUMMON_ATK_GROUND:
						if( re_baselv_bonus == 1 && s_level >= 100 )
						skillratio = (1 + skill_lv) / 2 * (s_level + s_job_level);
						else
						skillratio = (1 + skill_lv) / 2 * 200;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio += skillratio * (s_level - 100) / 200;	// Base level bonus.
						break;
					case LG_SHIELDSPELL:
					if ( sd && skill_lv == 2 )
						if( battle_config.renewal_baselvl_skill_effect == 1 && status_get_lv(src) >= 100 )
							skillratio = 4 * s_level + 100 * sd->shieldmdef + 2 * sstatus->int_;
						else
							skillratio = 600 + 100 * sd->shieldmdef + 2 * sstatus->int_;
					else
						skillratio = 0;//Prevents MATK damage from being done on LV 1 usage since LV 1 us ATK. [Rytech]
					break;
					case LG_RAYOFGENESIS:
						skillratio = 300 * skill_lv;
						if( sc && sc->data[SC_BANDING] && sc->data[SC_BANDING]->val2 > 1 )
							skillratio += 200 * sc->data[SC_BANDING]->val2;
						if( sc && sc->data[SC_INSPIRATION] )
							skillratio += 600;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_job_level / 25;	// Job level bonus.
						break;
					case WM_METALICSOUND:
						skillratio = 120 * skill_lv + 60 * pc_checkskill(sd, WM_LESSON);
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						break;
					case WM_REVERBERATION_MAGIC:
						skillratio += 100 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						if (mflag <= 0)// mflag holds the number of enemy targets in the AoE to divide the damage between.
							mflag = 1;// Make sure its always at least 1 to avoid divide by 0 crash.
						skillratio = skillratio / mflag;
						break;
					case SO_FIREWALK:
						skillratio = 60 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						if( sc && sc->data[SC_HEATER_OPTION] )
							skillratio += skillratio * sc->data[SC_HEATER_OPTION]->val3 / 100;
						break;
					case SO_ELECTRICWALK:
						skillratio = 60 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						if( sc && sc->data[SC_BLAST_OPTION] )
							skillratio += skillratio * sc->data[SC_BLAST_OPTION]->val2 / 100;
						break;
					case SO_EARTHGRAVE:
						skillratio = sstatus->int_ * skill_lv + 200 * pc_checkskill(sd, SA_SEISMICWEAPON);
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level/ 100;	// Base level bonus.
						if( sc && sc->data[SC_CURSED_SOIL_OPTION] )
							skillratio += skillratio * sc->data[SC_CURSED_SOIL_OPTION]->val2 / 100;
						break;
					case SO_DIAMONDDUST:
						skillratio = sstatus->int_ * skill_lv + 200 * pc_checkskill(sd, SA_FROSTWEAPON);
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						if( sc && sc->data[SC_COOLER_OPTION] )
							skillratio += skillratio * sc->data[SC_COOLER_OPTION]->val3 / 100;
						break;
					case SO_POISON_BUSTER:
						skillratio += 900 + 300 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 120;	// Base level bonus.
						if( sc && sc->data[SC_CURSED_SOIL_OPTION] )
							skillratio += skillratio * sc->data[SC_CURSED_SOIL_OPTION]->val2 / 100;
						break;
					case SO_PSYCHIC_WAVE:
						skillratio = 70 * skill_lv + 3 * sstatus->int_;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						if( sc )
						{
							if( sc->data[SC_HEATER_OPTION] )
								skillratio += skillratio * sc->data[SC_HEATER_OPTION]->val3 / 100;
							else if(sc->data[SC_COOLER_OPTION] )
								skillratio += skillratio * sc->data[SC_COOLER_OPTION]->val3 / 100;
							else if(sc->data[SC_BLAST_OPTION] )
								skillratio += skillratio * sc->data[SC_BLAST_OPTION]->val2 / 100;
							else if(sc->data[SC_CURSED_SOIL_OPTION] )
								skillratio += skillratio * sc->data[SC_CURSED_SOIL_OPTION]->val3 / 100;
						}
						break;
					case SO_CLOUD_KILL:
						skillratio = 40 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						if( sc && sc->data[SC_CURSED_SOIL_OPTION] )
							skillratio += skillratio * sc->data[SC_CURSED_SOIL_OPTION]->val2 / 100;
						break;
					case SO_VARETYR_SPEAR:
						skillratio = sstatus->int_ * skill_lv + 50 * pc_checkskill(sd, SA_LIGHTNINGLOADER);
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;	// Base level bonus.
						if( sc && sc->data[SC_BLAST_OPTION] )
							skillratio += skillratio * sc->data[SC_BLAST_OPTION]->val2 / 100;
						break;
					case GN_DEMONIC_FIRE:
						if ( skill_lv > 20 )// Fire Expansion Level 2
							skillratio += 10 + 20 * (skill_lv - 20) + 10 * sstatus->int_;
						else if ( skill_lv > 10 )// Fire Expansion Level 1
							if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio += 10 + 20 * (skill_lv - 10) + sstatus->int_ + status_get_job_lv(src);
							else
							skillratio += 10 + 20 * (skill_lv - 10) + sstatus->int_ + 50;
						else// Normal Demonic Fire Damage
							skillratio += 10 + 20 * skill_lv;
						break;
					case KO_KAIHOU:
						skillratio = 200 * sd->spiritballnumber;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;
						break;
					case MH_POISON_MIST:
						skillratio = 40 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 100;
						break;
					//Lv 1 - 600, Lv 2 - 1000, Lv 3 - 800, Lv 4 1200, Lv 5 - 1000
					case MH_ERASER_CUTTER:
						if ( skill_lv % 2 == 0 )//Even levels 2, 4, 6, 8, etc.
							skillratio = 800 + 100 * skill_lv;
						else//Odd levels 1, 3, 5, 7, etc.
							skillratio = 500 + 100 * skill_lv;
						break;
					//Lv 1 - 500, Lv 2 - 700, Lv 3 - 600, Lv 4 900, Lv 5 - 700
					case MH_XENO_SLASHER:
						if ( skill_lv % 2 == 0 )//Even levels 2, 4, 6, 8, etc.
							skillratio = 500 + 100 * skill_lv;
						else//Odd levels 1, 3, 5, 7, etc.
							skillratio = 450 + 50 * skill_lv;
						break;
					case MH_HEILIGE_STANGE:
						skillratio = 500 + 250 * skill_lv;
						if( re_baselv_bonus == 1 && s_level >= 100 )
							skillratio = skillratio * s_level / 150;
						break;
					// Magical Elemental Spirits Attack Skills
					case EL_FIRE_MANTLE:
					case EL_WATER_SCREW:
						skillratio += 900;
						break;
					case EL_FIRE_ARROW:
					case EL_ROCK_CRUSHER_ATK:
						skillratio += 200;
						break;
					case EL_FIRE_BOMB:
					case EL_ICE_NEEDLE:
					case EL_HURRICANE_ATK:
						skillratio += 400;
						break;
					case EL_FIRE_WAVE:
					case EL_TYPOON_MIS_ATK:
						skillratio += 1100;
						break;
				}

				if ( sc )
				{// Insignia's increases the damage of offensive magic by a fixed percentage depending on the element.
					if ((sc->data[SC_FIRE_INSIGNIA] && sc->data[SC_FIRE_INSIGNIA]->val1 == 3 && s_ele == ELE_FIRE) ||
						(sc->data[SC_WATER_INSIGNIA] && sc->data[SC_WATER_INSIGNIA]->val1 == 3 && s_ele == ELE_WATER) ||
						(sc->data[SC_WIND_INSIGNIA] && sc->data[SC_WIND_INSIGNIA]->val1 == 3 && s_ele == ELE_WIND) ||
						(sc->data[SC_EARTH_INSIGNIA] && sc->data[SC_EARTH_INSIGNIA]->val1 == 3 && s_ele == ELE_EARTH))
						skillratio += 25;
				}

				// The code below is disabled until I can get more accurate information on the skills that insignias
				// increase the damage on. Right now it appears that certain skills get the increase, but need more info.
				/*if ( sc )
				{// Insignia's appear to only increase the damage of certain magic skills and not all of a certain
				//  element like it claims to. Need a confirm on Ninja skills for sure. [Rytech]
					if (( sc->data[SC_FIRE_INSIGNIA] && sc->data[SC_FIRE_INSIGNIA]->val1 == 3 && (
						skill_num == MG_FIREBALL ||
						skill_num == MG_FIREWALL ||
						skill_num == MG_FIREBOLT ||
						skill_num == WZ_FIREPILLAR ||
						skill_num == WZ_SIGHTRASHER ||
						skill_num == WZ_METEOR ||
						skill_num == WL_CRIMSONROCK ||
						skill_num == WL_HELLINFERNO ||
						skill_num == WL_SUMMON_ATK_FIRE ||
						skill_num == SO_FIREWALK ))
						||
					( sc->data[SC_WATER_INSIGNIA] && sc->data[SC_WATER_INSIGNIA]->val1 == 3 && (
						skill_num == MG_COLDBOLT ||
						skill_num == MG_FROSTDIVER ||
						skill_num == WZ_WATERBALL ||
						skill_num == WZ_FROSTNOVA ||
						skill_num == WZ_STORMGUST ||
						skill_num == WL_FROSTMISTY ||
						skill_num == WL_JACKFROST ||
						skill_num == WL_SUMMON_ATK_WATER ||
						skill_num == SO_DIAMONDDUST ))
						||
					( sc->data[SC_WIND_INSIGNIA] && sc->data[SC_WIND_INSIGNIA]->val1 == 3 && (
						skill_num == MG_LIGHTNINGBOLT ||
						skill_num == MG_THUNDERSTORM ||
						skill_num == WZ_JUPITEL ||
						skill_num == WZ_VERMILION ||
						skill_num == WL_CHAINLIGHTNING_ATK ||
						skill_num == WL_SUMMON_ATK_WIND ||
						skill_num == SO_ELECTRICWALK ||
						skill_num == SO_VARETYR_SPEAR ))
						||
					( sc->data[SC_EARTH_INSIGNIA] && sc->data[SC_EARTH_INSIGNIA]->val1 == 3 && (
						skill_num == WZ_EARTHSPIKE ||
						skill_num == WZ_HEAVENDRIVE ||
						skill_num == WL_EARTHSTRAIN ||
						skill_num == WL_SUMMON_ATK_GROUND ||
						skill_num == SO_EARTHGRAVE )))
						skillratio += 25;
				}*/

				MATK_RATE(skillratio);
			
				//Constant/misc additions from skills
				if (skill_num == WZ_FIREPILLAR)
					MATK_ADD(50);
			}
		}

		if(sd) {
			//Damage bonuses
			if ((i = pc_skillatk_bonus(sd, skill_num)))
				ad.damage += ad.damage*i/100;

			//Ignore Defense?
			if (!flag.imdef && (
				sd->ignore_mdef_ele & (1<<tstatus->def_ele) ||
				sd->ignore_mdef_race & (1<<tstatus->race) ||
				sd->ignore_mdef_race & (is_boss(target)?1<<RC_BOSS:1<<RC_NONBOSS)
			))
				flag.imdef = 1;
		}

		if(!flag.imdef){
			char mdef = tstatus->mdef;
			int mdef2= tstatus->mdef2;
			if(sd) {
				i = sd->ignore_mdef[is_boss(target)?RC_BOSS:RC_NONBOSS];
				i+= sd->ignore_mdef[tstatus->race];
				if (i)
				{
					if (i > 100) i = 100;
					mdef -= mdef * i/100;
					//mdef2-= mdef2* i/100;
				}
			}
			if(battle_config.magic_defense_type)
				ad.damage = ad.damage - mdef*battle_config.magic_defense_type - mdef2;
			else
				ad.damage = ad.damage * (100-mdef)/100 - mdef2;
		}
		
		if (skill_num == NPC_EARTHQUAKE)
		{	//Adds atk2 to the damage, should be influenced by number of hits and skill-ratio, but not mdef reductions. [Skotlex]
			//Also divide the extra bonuses from atk2 based on the number in range [Kevin]
			if(mflag>0)
				ad.damage+= (sstatus->rhw.atk2*skillratio/100)/mflag;
			else
				ShowError("Zero range by %d:%s, divide per 0 avoided!\n", skill_num, skill_get_name(skill_num));
		}

		if(ad.damage<1)
			ad.damage=1;

		if (!(nk&NK_NO_ELEFIX))
			ad.damage=battle_attr_fix(src, target, ad.damage, s_ele, tstatus->def_ele, tstatus->ele_lv);

		if( skill_num == CR_GRANDCROSS || skill_num == NPC_GRANDDARKNESS )
		{ //Apply the physical part of the skill's damage. [Skotlex]
			struct Damage wd = battle_calc_weapon_attack(src,target,skill_num,skill_lv,mflag);
			ad.damage = battle_attr_fix(src, target, wd.damage + ad.damage, s_ele, tstatus->def_ele, tstatus->ele_lv) * (100 + 40*skill_lv)/100;
			if( src == target )
			{
				if( src->type == BL_PC )
					ad.damage = ad.damage/2;
				else
					ad.damage = 0;
			}
		}

		if (sd && !(nk&NK_NO_CARDFIX_ATK)) {
			short t_class = status_get_class(target);
			short cardfix=1000;

			cardfix=cardfix*(100+sd->magic_addrace[tstatus->race])/100;
			if (!(nk&NK_NO_ELEFIX))
				cardfix=cardfix*(100+sd->magic_addele[tstatus->def_ele])/100;
			cardfix=cardfix*(100+sd->magic_addsize[tstatus->size])/100;
			cardfix=cardfix*(100+sd->magic_addrace[is_boss(target)?RC_BOSS:RC_NONBOSS])/100;
			for(i=0; i< ARRAYLENGTH(sd->add_mdmg) && sd->add_mdmg[i].rate;i++) {
				if(sd->add_mdmg[i].class_ == t_class) {
					cardfix=cardfix*(100+sd->add_mdmg[i].rate)/100;
					continue;
				}
			}
			if (cardfix != 1000)
				MATK_RATE(cardfix/10);
		}

		if( tsd && !(nk&NK_NO_CARDFIX_DEF) )
	  	{ // Target cards.
			short s_race2 = status_get_race2(src);
			short s_class= status_get_class(src);
			int cardfix=1000;

			if (!(nk&NK_NO_ELEFIX))
			{
				int ele_fix = tsd->subele[s_ele];
				for (i = 0; ARRAYLENGTH(tsd->subele2) > i && tsd->subele2[i].rate != 0; i++)
				{
					if(tsd->subele2[i].ele != s_ele) continue;
					if(!(tsd->subele2[i].flag&ad.flag&BF_WEAPONMASK &&
						 tsd->subele2[i].flag&ad.flag&BF_RANGEMASK &&
						 tsd->subele2[i].flag&ad.flag&BF_SKILLMASK))
						continue;
					ele_fix += tsd->subele2[i].rate;
				}
				cardfix=cardfix*(100-ele_fix)/100;
			}
			cardfix=cardfix*(100-tsd->subsize[sstatus->size])/100;
			cardfix=cardfix*(100-tsd->subrace2[s_race2])/100;
			cardfix=cardfix*(100-tsd->subrace[sstatus->race])/100;
			cardfix=cardfix*(100-tsd->subrace[is_boss(src)?RC_BOSS:RC_NONBOSS])/100;
			if( sstatus->race != RC_DEMIHUMAN )
				cardfix=cardfix*(100-tsd->subrace[RC_NONDEMIHUMAN])/100;

			for(i=0; i < ARRAYLENGTH(tsd->add_mdef) && tsd->add_mdef[i].rate;i++) {
				if(tsd->add_mdef[i].class_ == s_class) {
					cardfix=cardfix*(100-tsd->add_mdef[i].rate)/100;
					break;
				}
			}
			//It was discovered that ranged defense also counts vs magic! [Skotlex]
			if (ad.flag&BF_SHORT)
				cardfix=cardfix*(100-tsd->near_attack_def_rate)/100;
			else
				cardfix=cardfix*(100-tsd->long_attack_def_rate)/100;

			cardfix=cardfix*(100-tsd->magic_def_rate)/100;

			if( tsd->sc.data[SC_MDEF_RATE] )
				cardfix=cardfix*(100-tsd->sc.data[SC_MDEF_RATE]->val1)/100;

			if (cardfix != 1000)
				MATK_RATE(cardfix/10);
		}
	}

	damage_div_fix(ad.damage, ad.div_);
	
	if (flag.infdef && ad.damage)
	{
		ad.damage = ad.damage>0?1:-1;
		if( skill_num == WL_JACKFROST || skill_num == WL_FROSTMISTY )
		{
			ad.damage = 0;
			ad.dmg_lv = ATK_MISS;
		}
	}

	ad.damage = battle_calc_damage(src,target,&ad,ad.damage,skill_num,skill_lv,s_ele);
	if( map_flag_gvg2(target->m) )
		ad.damage = battle_calc_gvg_damage(src,target,ad.damage,ad.div_,skill_num,skill_lv,ad.flag);
	else if( map[target->m].flag.battleground )
		ad.damage = battle_calc_bg_damage(src,target,ad.damage,ad.div_,skill_num,skill_lv,ad.flag);

	if( skill_num == WL_HELLINFERNO && s_ele == ELE_FIRE )
	{ // Calculates Shadow Element Extra
		struct Damage md = battle_calc_magic_attack(src,target,skill_num,-skill_lv,mflag);
		ad.damage += md.damage;
	}

	if( skill_num == SO_VARETYR_SPEAR )
	{ // Physical damage.
		struct Damage wd = battle_calc_weapon_attack(src,target,skill_num,skill_lv,mflag);
		ad.damage += wd.damage;
	}

	return ad;
}

/*==========================================
 * ���̑��_??[�W�v�Z
 *------------------------------------------*/
struct Damage battle_calc_misc_attack(struct block_list *src,struct block_list *target,int skill_num,int skill_lv,int mflag)
{
	int skill, s_level;
	short i, nk;
	short s_ele;
	char re_baselv_bonus = battle_config.renewal_baselvl_skill_ratio;//BaseLv config for renewal skills.
	int chorusbonus = 0;//Chorus bonus value for chorus skills. Bonus remains 0 unless 3 or more Minstrel's/Wanderer's are in the party.

	struct map_session_data *sd, *tsd;
	struct Damage md; //DO NOT CONFUSE with md of mob_data!
	struct status_data *sstatus = status_get_status_data(src);
	struct status_data *tstatus = status_get_status_data(target);

	memset(&md,0,sizeof(md));

	if( src == NULL || target == NULL ){
		nullpo_info(NLP_MARK);
		return md;
	}

	//Some initial values
	md.amotion=skill_get_inf(skill_num)&INF_GROUND_SKILL?0:sstatus->amotion;
	md.dmotion=tstatus->dmotion;
	md.div_=skill_get_num( skill_num,skill_lv );
	md.blewcount=skill_get_blewcount(skill_num,skill_lv);
	md.dmg_lv=ATK_DEF;
	md.flag=BF_MISC|BF_SKILL;

	nk = skill_get_nk(skill_num);
	
	sd = BL_CAST(BL_PC, src);
	tsd = BL_CAST(BL_PC, target);

	// Minstrel/Wanderer number check for chorus skills.
	// Bonus remains 0 unless 3 or more Minstrel's/Wanderer's are in the party.
	if( sd && sd->status.party_id && party_foreachsamemap(party_sub_count_chorus, sd, 0) > 7)
		chorusbonus = 5;//Maximum effect possiable from 7 or more Minstrel's/Wanderer's
	else if( sd && sd->status.party_id && party_foreachsamemap(party_sub_count_chorus, sd, 0) > 2)
		chorusbonus = party_foreachsamemap(party_sub_count_chorus, sd, 0) - 2;//Effect bonus from additional Minstrel's/Wanderer's if not above the max possiable.

	if(sd) {
		sd->state.arrow_atk = 0;
		md.blewcount += battle_blewcount_bonus(sd, skill_num);
	}

	s_ele = skill_get_ele(skill_num, skill_lv);
	if (s_ele < 0 && s_ele != -3) //Attack that takes weapon's element for misc attacks? Make it neutral [Skotlex]
		s_ele = ELE_NEUTRAL;
	else if (s_ele == -3) //Use random element
		s_ele = rand()%ELE_MAX;

	//Skill Range Criteria
	md.flag |= battle_range_type(src, target, skill_num, skill_lv);
	s_level = status_get_lv(src);

	switch( skill_num )
	{
	case HT_LANDMINE:
	case MA_LANDMINE:
		md.damage = skill_lv * (sstatus->dex + 75) * (100 + sstatus->int_) / 100;
		break;
	case HT_BLASTMINE:
		md.damage = skill_lv * (sstatus->dex / 2 + 50)*(100 + sstatus->int_) / 100;
		break;
	case HT_CLAYMORETRAP:
		md.damage = skill_lv * (sstatus->dex / 2 + 75)*(100 + sstatus->int_) / 100;
		break;
	case HT_BLITZBEAT:
	case SN_FALCONASSAULT:
		//Blitz-beat Damage.
		if(!sd || (skill = pc_checkskill(sd,HT_STEELCROW)) <= 0)
			skill=0;
		md.damage=(sstatus->dex/10+sstatus->int_/2+skill*3+40)*2;
		if(mflag > 1) //Autocasted Blitz.
			nk|=NK_SPLASHSPLIT;
		
		if (skill_num == SN_FALCONASSAULT)
		{
			//Div fix of Blitzbeat
			skill = skill_get_num(HT_BLITZBEAT, 5);
			damage_div_fix(md.damage, skill); 

			//Falcon Assault Modifier
			md.damage=md.damage*(150+70*skill_lv)/100;
		}
		break;
	case TF_THROWSTONE:
		md.damage=50;
		break;
	case BA_DISSONANCE:
		md.damage=30+skill_lv*10;
		if (sd)
			md.damage+= 3*pc_checkskill(sd,BA_MUSICALLESSON);
		break;
	case NPC_SELFDESTRUCTION:
		md.damage = sstatus->hp;
		break;
	case NPC_SMOKING:
		md.damage=3;
		break;
	case NPC_DARKBREATH:
		md.damage = 500 + (skill_lv-1)*1000 + rand()%1000;
		if(md.damage > 9999) md.damage = 9999;
		break;
	case PA_PRESSURE:
		md.damage=500+300*skill_lv;
		break;
	case PA_GOSPEL:
		md.damage = 1+rand()%9999;
		break;
	case CR_ACIDDEMONSTRATION: // updated the formula based on a Japanese formula found to be exact [Reddozen]
		if(tstatus->vit+sstatus->int_) //crash fix
			md.damage = (int)((int64)7*tstatus->vit*sstatus->int_*sstatus->int_ / (10*(tstatus->vit+sstatus->int_)));
		else
			md.damage = 0;
		if (tsd) md.damage>>=1;
		if (md.damage < 0 || md.damage > INT_MAX>>1)
	  	//Overflow prevention, will anyone whine if I cap it to a few billion?
		//Not capped to INT_MAX to give some room for further damage increase.
			md.damage = INT_MAX>>1;
		break;
	case NJ_ZENYNAGE:
		md.damage = skill_get_zeny(skill_num ,skill_lv);
		if (!md.damage) md.damage = 2;
		md.damage = md.damage + rand()%md.damage;
		if (is_boss(target))
			md.damage=md.damage/3;
		else if (tsd)
			md.damage=md.damage/2;
		break;
	case GS_FLING:
		md.damage = sd ? sd->status.job_level : status_get_lv(src);
		break;
	case HVAN_EXPLOSION:	//[orn]
		md.damage = sstatus->max_hp * (50 + 50 * skill_lv) / 100 ;
		break ;
	case ASC_BREAKER:
		md.damage = 500 + rand()%500 + 5 * skill_lv * sstatus->int_;
		nk|=NK_IGNORE_FLEE|NK_NO_ELEFIX; //These two are not properties of the weapon based part.
		break;
	case HW_GRAVITATION:
		md.damage = 200 + 200 * skill_lv;
		md.dmotion = 0; //No flinch animation.
		break;
	case NPC_EVILLAND:
		md.damage = skill_calc_heal(src,target,skill_num,skill_lv,false);
		break;
	case RA_CLUSTERBOMB:
	case RA_FIRINGTRAP:
 	case RA_ICEBOUNDTRAP:
		md.damage = skill_lv * sstatus->dex + 5 * sstatus->int_;
		if ( re_baselv_bonus == 1 && s_level >= 100 )
			md.damage = md.damage * 150 / 100 + md.damage * s_level / 100;// Base level bonus.
		else
			md.damage = md.damage * 250 / 100;
		md.damage = md.damage * (20 * pc_checkskill(sd,RA_RESEARCHTRAP)) / 50;
		break;
	case WM_SOUND_OF_DESTRUCTION:
		md.damage = 1000 * skill_lv + sstatus->int_ * pc_checkskill(sd,WM_LESSON);
		md.damage += md.damage * ( 10 * chorusbonus ) / 100;
		break;
	//case WM_SATURDAY_NIGHT_FEVER://Test me in official if possiable.
	//	md.damage = 9999;//To enable when I figure how it exactly applies the damage. For now clif damage will deal 9999 damage and display it. [Rytech]
	//	break;
	case GN_THORNS_TRAP:
		md.damage = 100 + 200 * skill_lv + sstatus->int_;
		break;
	case GN_BLOOD_SUCKER:
		md.damage = 200 + 100 * skill_lv + sstatus->int_;
		break;
	case GN_HELLS_PLANT_ATK:
		if ( re_baselv_bonus == 1 )
		md.damage = 10 * skill_lv * status_get_lv(target) + 7 * sstatus->int_ / 2 * ( status_get_job_lv(src) / 4 + 18 ) * 5 / ( 10 - pc_checkskill(sd,AM_CANNIBALIZE) );
		else
		md.damage = 10 * skill_lv * status_get_lv(target) + 7 * sstatus->int_ / 2 * 30 * 5 / ( 10 - pc_checkskill(sd,AM_CANNIBALIZE) );
		break;
	case KO_MUCHANAGE:
		md.damage = skill_get_zeny(skill_num ,skill_lv);
		if (!md.damage) md.damage = 10;
		md.damage =  md.damage * rnd_value( 50, 100) / 100;
		if (is_boss(target) || (tsd))
			md.damage = md.damage / 2;
		break;
	case MH_EQC:
		md.damage = tstatus->hp - sstatus->hp;
		// Officially, if damage comes out <= 0,
		// the damage will equal to the homunculus MaxHP.
		// Bug? I think so and im not adding that.
		if ( md.damage < 0 )
			md.damage = 1;
		break;
	}

	if (nk&NK_SPLASHSPLIT){ // Divide ATK among targets
		if(mflag>0)
			md.damage/= mflag;
		else
			ShowError("0 enemies targeted by %d:%s, divide per 0 avoided!\n", skill_num, skill_get_name(skill_num));
	}

	damage_div_fix(md.damage, md.div_);
	
	if (!(nk&NK_IGNORE_FLEE))
	{
		struct status_change *sc = status_get_sc(target);
		i = 0; //Temp for "hit or no hit"
		if(sc && sc->opt1 && sc->opt1 != OPT1_STONEWAIT)
			i = 1;
		else {
			short
				flee = tstatus->flee,
				hitrate=80; //Default hitrate

			if(battle_config.agi_penalty_type && 
				battle_config.agi_penalty_target&target->type)
			{	
				unsigned char attacker_count; //256 max targets should be a sane max
				attacker_count = unit_counttargeted(target,battle_config.agi_penalty_count_lv);
				if(attacker_count >= battle_config.agi_penalty_count)
				{
					if (battle_config.agi_penalty_type == 1)
						flee = (flee * (100 - (attacker_count - (battle_config.agi_penalty_count - 1))*battle_config.agi_penalty_num))/100;
					else //asume type 2: absolute reduction
						flee -= (attacker_count - (battle_config.agi_penalty_count - 1))*battle_config.agi_penalty_num;
					if(flee < 1) flee = 1;
				}
			}

			hitrate+= sstatus->hit - flee;
			hitrate = cap_value(hitrate, battle_config.min_hitrate, battle_config.max_hitrate);

			if(rand()%100 < hitrate)
				i = 1;
		}
		if (!i) {
			md.damage = 0;
			md.dmg_lv=ATK_FLEE;
		}
	}

	if( md.damage && tsd && !(nk&NK_NO_CARDFIX_DEF) )
	{// misc damage reduction from equipment
		int cardfix = 10000;
		int race2 = status_get_race2(src);
		if (!(nk&NK_NO_ELEFIX))
		{
			int ele_fix = tsd->subele[s_ele];
			for (i = 0; ARRAYLENGTH(tsd->subele2) > i && tsd->subele2[i].rate != 0; i++)
			{
				if(tsd->subele2[i].ele != s_ele) continue;
				if(!(tsd->subele2[i].flag&md.flag&BF_WEAPONMASK &&
					 tsd->subele2[i].flag&md.flag&BF_RANGEMASK &&
					 tsd->subele2[i].flag&md.flag&BF_SKILLMASK))
					continue;
				ele_fix += tsd->subele2[i].rate;
			}
			cardfix=cardfix*(100-ele_fix)/100;
		}
		cardfix=cardfix*(100-tsd->subsize[sstatus->size])/100;
		cardfix=cardfix*(100-tsd->subrace2[race2])/100;
		cardfix=cardfix*(100-tsd->subrace[sstatus->race])/100;
		cardfix=cardfix*(100-tsd->subrace[is_boss(src)?RC_BOSS:RC_NONBOSS])/100;
		if( sstatus->race != RC_DEMIHUMAN )
			cardfix=cardfix*(100-tsd->subrace[RC_NONDEMIHUMAN])/100;

		cardfix=cardfix*(100-tsd->misc_def_rate)/100;
		if(md.flag&BF_SHORT)
			cardfix=cardfix*(100-tsd->near_attack_def_rate)/100;
		else	// BF_LONG (there's no other choice)
			cardfix=cardfix*(100-tsd->long_attack_def_rate)/100;

		if (cardfix != 10000)
			md.damage=(int)((int64)md.damage*cardfix/10000);
	}

	if (sd && (i = pc_skillatk_bonus(sd, skill_num)))
		md.damage += md.damage*i/100;

	if(md.damage < 0)
		md.damage = 0;
	else if(md.damage && tstatus->mode&MD_PLANT)
		md.damage = 1;

	if( target->type == BL_SKILL && ((TBL_SKILL*)target)->group->unit_id == UNT_REVERBERATION )
		md.damage = 1; // Reberberation takes 1 damage

	if(!(nk&NK_NO_ELEFIX))
		md.damage=battle_attr_fix(src, target, md.damage, s_ele, tstatus->def_ele, tstatus->ele_lv);

	md.damage = battle_calc_damage(src,target,&md,md.damage,skill_num,skill_lv,s_ele);
	if( map_flag_gvg2(target->m) )
		md.damage = battle_calc_gvg_damage(src,target,md.damage,md.div_,skill_num,skill_lv,md.flag);
	else if( map[target->m].flag.battleground )
		md.damage = battle_calc_bg_damage(src,target,md.damage,md.div_,skill_num,skill_lv,md.flag);

	switch( skill_num )
	{
		case RA_CLUSTERBOMB:
		case RA_FIRINGTRAP:
 		case RA_ICEBOUNDTRAP:
			{
				struct Damage wd;

				wd = battle_calc_weapon_attack(src,target,skill_num,skill_lv,mflag);
				md.damage += wd.damage;
			}
			break;
		case NJ_ZENYNAGE:
			if( sd )
			{
				if ( md.damage > sd->status.zeny )
					md.damage = sd->status.zeny;
				pc_payzeny(sd, md.damage);
			}
		break;
	}

	return md;
}
/*==========================================
 * �_??[�W�v�Z�ꊇ?��?�p
 *------------------------------------------*/
struct Damage battle_calc_attack(int attack_type,struct block_list *bl,struct block_list *target,int skill_num,int skill_lv,int count)
{
	struct Damage d;
	switch( attack_type )
	{
		case BF_WEAPON: d = battle_calc_weapon_attack(bl,target,skill_num,skill_lv,count); break;
		case BF_MAGIC:  d = battle_calc_magic_attack(bl,target,skill_num,skill_lv,count);  break;
		case BF_MISC:   d = battle_calc_misc_attack(bl,target,skill_num,skill_lv,count);   break;
		default:
			ShowError("battle_calc_attack: unknown attack type! %d\n",attack_type);
			memset(&d,0,sizeof(d));
			break;
	}
	if( d.damage + d.damage2 < 1 )
	{	//Miss/Absorbed
		//Weapon attacks should go through to cause additional effects.
		if (d.dmg_lv == ATK_DEF /*&& attack_type&(BF_MAGIC|BF_MISC)*/) // Isn't it that additional effects don't apply if miss?
			d.dmg_lv = ATK_MISS;
		d.dmotion = 0;
	}
	else // Some skills like Weaponry Research will cause damage even if attack is dodged
		d.dmg_lv = ATK_DEF;
	return d;
}

//Calculates BF_WEAPON returned damage.
int battle_calc_return_damage(struct block_list *src, struct block_list *bl, int *damage, int flag)
{
	struct map_session_data* sd = NULL;
	int rdamage = 0, max_damage = status_get_max_hp(bl);
	struct status_change *sc = status_get_sc(bl);
	struct status_change *ssc = status_get_sc(src);

	sd = BL_CAST(BL_PC, bl);

	//Bounces back part of the damage.
	if( (flag&(BF_SHORT|BF_MAGIC)) == BF_SHORT )
	{
		if( sd && sd->short_weapon_damage_return )
		{
			rdamage += (*damage) * sd->short_weapon_damage_return / 100;
			if(rdamage < 1) rdamage = 1;
		}
		if( sc && sc->data[SC_DEATHBOUND] && !is_boss(src) )
		{
			int dir = map_calc_dir(bl,src->x,src->y),
				t_dir = unit_getdir(bl), rd1 = 0;

			if( distance_bl(src,bl) <= 0 || !map_check_dir(dir,t_dir) )
			{
				rd1 = min((*damage),max_damage) * sc->data[SC_DEATHBOUND]->val2 / 100; // Amplify damage.
				(*damage) = rd1 * 30 / 100; // Player receives 30% of the amplified damage.
				clif_skill_damage(src,bl,gettick(), status_get_amotion(src), 0, -30000, 1, RK_DEATHBOUND, sc->data[SC_DEATHBOUND]->val1,6);
				status_change_end(bl,SC_DEATHBOUND,-1);
				rdamage += rd1 * 70 / 100; // Target receives 70% of the amplified damage. [Rytech]
			}
		}
		if( sc && sc->data[SC_REFLECTSHIELD] )
		{
			rdamage += (*damage) * sc->data[SC_REFLECTSHIELD]->val2 / 100;
			if (rdamage < 1) rdamage = 1;
		}//Now only reflects short range damage only. Does not reflect magic anymore.
		if( sc && sc->data[SC_REFLECTDAMAGE] && rand()%100 < 30 + 10 * sc->data[SC_REFLECTDAMAGE]->val1)
		{
			if( battle_config.renewal_baselvl_skill_effect == 1 && status_get_lv(src) >= 100 )
				max_damage = max_damage * status_get_lv(bl) / 100;
			else
				max_damage = max_damage * 150 / 100;
			rdamage = (*damage) * sc->data[SC_REFLECTDAMAGE]->val2 / 100;
			if( rdamage > max_damage ) rdamage = max_damage;
			if ((--sc->data[SC_REFLECTDAMAGE]->val3) <= 0)
				status_change_end(bl,SC_REFLECTDAMAGE,-1);
		}
		if( sc && sc->data[SC_SHIELDSPELL_DEF] && sc->data[SC_SHIELDSPELL_DEF]->val1 == 2 )
		{
			rdamage += (*damage) * sc->data[SC_SHIELDSPELL_DEF]->val2 / 100;
			rdamage = cap_value(rdamage,1,max_damage);
		}
		if( ssc && ssc->data[SC_INSPIRATION] )
		{
			rdamage += (*damage) / 100;
			rdamage = cap_value(rdamage,1,max_damage);
		}
		if( sc && sc->data[SC_CRESCENTELBOW] && !(flag&BF_SKILL) && !is_boss(src) && rand()%100 < sc->data[SC_CRESCENTELBOW]->val2 )
		{
			//rdamage += (int)((*damage) + (*damage) * status_get_hp(src) * 2.15 / 100000);//No longer used since its not official, but keeping for reference.
			rdamage += (*damage) * (100 + 20 * sc->data[SC_CRESCENTELBOW]->val1) / 100;//Part of the official formula. Will code the rest later. [Rytech]
			if( rdamage < 1 ) rdamage = 1;
		}
	}
	else
	{
		if( sd && sd->long_weapon_damage_return )
		{
			rdamage += (*damage) * sd->long_weapon_damage_return / 100;
			if( rdamage < 1 ) rdamage = 1;
		}
	}
	if ( sc && sc->data[SC_KYOMU] )//If under shadow void status, damage will not be reflected.
		rdamage = 0;
	return rdamage;
}

void battle_drain(TBL_PC *sd, struct block_list *tbl, int rdamage, int ldamage, int race, int boss)
{
	struct weapon_data *wd;
	int type, thp = 0, tsp = 0, rhp = 0, rsp = 0, hp, sp, i, *damage;
	for (i = 0; i < 4; i++) {
		//First two iterations: Right hand
		if (i < 2) { wd = &sd->right_weapon; damage = &rdamage; }
		else { wd = &sd->left_weapon; damage = &ldamage; }
		if (*damage <= 0) continue;
		//First and Third iterations: race, other two boss/nonboss state
		if (i == 0 || i == 2) 
			type = race;
		else
			type = boss?RC_BOSS:RC_NONBOSS;
		
		hp = wd->hp_drain[type].value;
		if (wd->hp_drain[type].rate)
			hp += battle_calc_drain(*damage, wd->hp_drain[type].rate, wd->hp_drain[type].per);

		sp = wd->sp_drain[type].value;
		if (wd->sp_drain[type].rate)
			sp += battle_calc_drain(*damage, wd->sp_drain[type].rate, wd->sp_drain[type].per);

		if (hp) {
			if (wd->hp_drain[type].type)
				rhp += hp;
			thp += hp;
		}
		if (sp) {
			if (wd->sp_drain[type].type)
				rsp += sp;
			tsp += sp;
		}
	}

	if (sd->sp_vanish_rate && rand()%1000 < sd->sp_vanish_rate)
		status_percent_damage(&sd->bl, tbl, 0, (unsigned char)sd->sp_vanish_per, false);
	if (!thp && !tsp) return;

	status_heal(&sd->bl, thp, tsp, battle_config.show_hp_sp_drain?3:1);
	
	if (rhp || rsp)
		status_zap(tbl, rhp, rsp);
}

// Deals the same damage to targets in area. [pakpil]
int battle_damage_area( struct block_list *bl, va_list ap)
{
	unsigned int tick;
	int amotion, dmotion, damage;
	struct block_list *src;

	nullpo_ret(bl);
	
	tick=va_arg(ap, unsigned int);
	src=va_arg(ap,struct block_list *);
	amotion=va_arg(ap,int);
	dmotion=va_arg(ap,int);
	damage=va_arg(ap,int);

	if( bl != src && battle_check_target(src,bl,BCT_ENEMY) > 0 && !(bl->type == BL_MOB && ((TBL_MOB*)bl)->class_ == MOBID_EMPERIUM) )
	{
		map_freeblock_lock();
		if( amotion )
			battle_delay_damage(tick, amotion,src,bl,0,0,0,damage,ATK_DEF,0);
		else
			status_fix_damage(src,bl,damage,0);
		clif_damage(bl,bl,tick,amotion,dmotion,damage,1,ATK_BLOCK,0);
		skill_additional_effect(src, bl, CR_REFLECTSHIELD, 1, BF_WEAPON|BF_SHORT|BF_NORMAL,ATK_DEF,tick);
		map_freeblock_unlock();
	}
	
	return 0;
}

/*==========================================
 * ��??U��?��?�܂Ƃ�
 *------------------------------------------*/
enum damage_lv battle_weapon_attack(struct block_list* src, struct block_list* target, unsigned int tick, int flag)
{
	struct map_session_data *sd = NULL, *tsd = NULL;
	struct homun_data *hd = NULL;
	struct status_data *sstatus, *tstatus;
	struct status_change *sc, *tsc;
	struct status_change_entry *sce;
	int damage,rdamage=0,rdelay=0;
	int skillv;
	struct Damage wd;

	nullpo_retr(ATK_NONE, src);
	nullpo_retr(ATK_NONE, target);

	if (src->prev == NULL || target->prev == NULL)
		return ATK_NONE;

	sd = BL_CAST(BL_PC, src);
	tsd = BL_CAST(BL_PC, target);
	hd = BL_CAST(BL_HOM, src);

	sstatus = status_get_status_data(src);
	tstatus = status_get_status_data(target);

	sc = status_get_sc(src);
	tsc = status_get_sc(target);

	if (sc && !sc->count) //Avoid sc checks when there's none to check for. [Skotlex]
		sc = NULL;
	if (tsc && !tsc->count)
		tsc = NULL;
	
	if (sd)
	{
		sd->state.arrow_atk = (sd->status.weapon == W_BOW || (sd->status.weapon >= W_REVOLVER && sd->status.weapon <= W_GRENADE));
		if (sd->state.arrow_atk)
		{
			int index = sd->equip_index[EQI_AMMO];
			if (index<0) {
				clif_arrow_fail(sd,0);
				return ATK_NONE;
			}
			//Ammo check by Ishizu-chan
			if (sd->inventory_data[index])
			switch (sd->status.weapon) {
			case W_BOW:
				if (sd->inventory_data[index]->look != A_ARROW) {
					clif_arrow_fail(sd,0);
					return ATK_NONE;
				}
			break;
			case W_REVOLVER:
			case W_RIFLE:
			case W_GATLING:
			case W_SHOTGUN:
				if (sd->inventory_data[index]->look != A_BULLET) {
					clif_arrow_fail(sd,0);
					return ATK_NONE;
				}
			break;
			case W_GRENADE:
				if (sd->inventory_data[index]->look != A_GRENADE) {
					clif_arrow_fail(sd,0);
					return ATK_NONE;
				}
			break;
			}
		}
	}

	if (sc && sc->data[SC_CLOAKING] && !(sc->data[SC_CLOAKING]->val4&2))
		status_change_end(src, SC_CLOAKING, INVALID_TIMER);

	if (sc && sc->data[SC_CLOAKINGEXCEED] && !(sc->data[SC_CLOAKINGEXCEED]->val4&2))
		status_change_end(src,SC_CLOAKINGEXCEED,-1);

	if (sc && sc->data[SC_CAMOUFLAGE] && !(sc->data[SC_CAMOUFLAGE]->val3&2))
		status_change_end(src,SC_CAMOUFLAGE,-1);

	if( tsc && tsc->data[SC_AUTOCOUNTER] && status_check_skilluse(target, src, KN_AUTOCOUNTER, tsc->data[SC_AUTOCOUNTER]->val1, 1) )
	{
		int dir = map_calc_dir(target,src->x,src->y);
		int t_dir = unit_getdir(target);
		int dist = distance_bl(src, target);
		if(dist <= 0 || (!map_check_dir(dir,t_dir) && dist <= tstatus->rhw.range+1))
		{
			int skilllv = tsc->data[SC_AUTOCOUNTER]->val1;
			clif_skillcastcancel(target); //Remove the casting bar. [Skotlex]
			clif_damage(src, target, tick, sstatus->amotion, 1, 0, 1, 0, 0); //Display MISS.
			status_change_end(target, SC_AUTOCOUNTER, INVALID_TIMER);
			skill_attack(BF_WEAPON,target,target,src,KN_AUTOCOUNTER,skilllv,tick,0);
			return ATK_NONE;
		}
	}

	if( tsc && tsc->data[SC_BLADESTOP_WAIT] && !is_boss(src) && (src->type == BL_PC || tsd == NULL || distance_bl(src, target) <= (tsd->status.weapon == W_FIST ? 1 : 2)) )
	{
		int skilllv = tsc->data[SC_BLADESTOP_WAIT]->val1;
		int duration = skill_get_time2(MO_BLADESTOP,skilllv);
		status_change_end(target, SC_BLADESTOP_WAIT, INVALID_TIMER);
		if(sc_start4(src, SC_BLADESTOP, 100, sd?pc_checkskill(sd, MO_BLADESTOP):5, 0, 0, target->id, duration))
	  	{	//Target locked.
			clif_damage(src, target, tick, sstatus->amotion, 1, 0, 1, 0, 0); //Display MISS.
			clif_bladestop(target, src->id, 1);
			sc_start4(target, SC_BLADESTOP, 100, skilllv, 0, 0, src->id, duration);
			return ATK_NONE;
		}
	}

	if(sd && (skillv = pc_checkskill(sd,MO_TRIPLEATTACK)) > 0)
	{
		int triple_rate= 30 - skillv; //Base Rate
		if (sc && sc->data[SC_SKILLRATE_UP] && sc->data[SC_SKILLRATE_UP]->val1 == MO_TRIPLEATTACK)
		{
			triple_rate+= triple_rate*(sc->data[SC_SKILLRATE_UP]->val2)/100;
			status_change_end(src, SC_SKILLRATE_UP, INVALID_TIMER);
		}
		if (rand()%100 < triple_rate)
			//FIXME: invalid return type!
			return (damage_lv)skill_attack(BF_WEAPON,src,src,target,MO_TRIPLEATTACK,skillv,tick,0);
	}

	if (sc)
	{
		if (sc->data[SC_SACRIFICE])
		{
			int skilllv = sc->data[SC_SACRIFICE]->val1;

			if( --sc->data[SC_SACRIFICE]->val2 <= 0 )
				status_change_end(src, SC_SACRIFICE, INVALID_TIMER);

			status_zap(src, sstatus->max_hp*9/100, 0);//Damage to self is always 9%

			//FIXME: invalid return type!
			return (damage_lv)skill_attack(BF_WEAPON,src,src,target,PA_SACRIFICE,skilllv,tick,0);
		}
		if (sc->data[SC_MAGICALATTACK])
			//FIXME: invalid return type!
			return (damage_lv)skill_attack(BF_MAGIC,src,src,target,NPC_MAGICALATTACK,sc->data[SC_MAGICALATTACK]->val1,tick,0);

		if ( sd && (sce = sc->data[SC_GT_ENERGYGAIN]) && rand()%100 < sce->val2 )
		{
			short spheremax = 5;

			if ( sc->data[SC_RAISINGDRAGON] )
				spheremax += sc->data[SC_RAISINGDRAGON]->val1;

			pc_addspiritball(sd, skill_get_time2(SR_GENTLETOUCH_ENERGYGAIN,sce->val1), spheremax);
		}

		if ( sc->data[SC_CRUSHSTRIKE] )
		{
			skill_attack(BF_WEAPON,src,src,target,RK_CRUSHSTRIKE,sc->data[SC_CRUSHSTRIKE]->val1,tick,flag);
			status_change_end(src, SC_CRUSHSTRIKE, -1);
			return ATK_MISS;
		}

		if ( hd && (sce = sc->data[SC_STYLE_CHANGE]) && sce->val1 == FIGHTER_STYLE && rand()%100 < sce->val2 )
			merc_hom_addspiritball(hd,MAX_HOMUN_SPHERES);
	}

	if(tsc && tsc->data[SC_KAAHI] && tsc->data[SC_KAAHI]->val4 == INVALID_TIMER && tstatus->hp < tstatus->max_hp)
		tsc->data[SC_KAAHI]->val4 = add_timer(tick + skill_get_time2(SL_KAAHI,tsc->data[SC_KAAHI]->val1), kaahi_heal_timer, target->id, SC_KAAHI); //Activate heal.

	wd = battle_calc_attack(BF_WEAPON, src, target, 0, 0, flag);

	if(sc)
	{
		if( sc->data[SC_EXEEDBREAK] )
		{
			wd.damage = wd.damage * sc->data[SC_EXEEDBREAK]->val1 / 100;
			status_change_end(src,SC_EXEEDBREAK,-1);
		}
		if( sc->data[SC_SPELLFIST] )
		{
			if( --(sc->data[SC_SPELLFIST]->val1) >= 0 )
				wd = battle_calc_attack(BF_MAGIC,src,target,sc->data[SC_SPELLFIST]->val3,sc->data[SC_SPELLFIST]->val4,flag);
			else
				status_change_end(src,SC_SPELLFIST,-1);
		}
	}

	if( sd && sc && sc->data[SC_GIANTGROWTH] && (wd.flag&BF_SHORT) && rand()%100 < sc->data[SC_GIANTGROWTH]->val2 )
		wd.damage *= 3; // Triple Damage

	if (sd && sd->state.arrow_atk) //Consume arrow.
		battle_consume_ammo(sd, 0, 0);

	damage = wd.damage + wd.damage2;
	if( damage > 0 && src != target )
	{
		
		if( sc && sc->data[SC_DUPLELIGHT] && (wd.flag&BF_SHORT) && rand()%100 <= 10+2*sc->data[SC_DUPLELIGHT]->val1 )
		{	// Activates it only from melee damage
			int skillid;
			if( rand()%2 == 1 )
				skillid = AB_DUPLELIGHT_MELEE;
			else
				skillid = AB_DUPLELIGHT_MAGIC;
			skill_attack(skill_get_type(skillid), src, src, target, skillid, sc->data[SC_DUPLELIGHT]->val1, tick, SD_LEVEL);
		}

		rdamage = battle_calc_return_damage(src, target, &damage, wd.flag);

		if( rdamage > 0 )
		{
			if( tsc && tsc->data[SC_REFLECTDAMAGE] )
			{
				if( src != target )// Don't reflect your own damage (Grand Cross)
					map_foreachinshootrange(battle_damage_area,target,skill_get_splash(LG_REFLECTDAMAGE,1),BL_CHAR,tick,target,wd.amotion,wd.dmotion,rdamage,tstatus->race,0);
			}
			else
			{
				if( tsc && tsc->data[SC_CRESCENTELBOW] )
				{	// Deal rdamage to src and 10% damage back to target.
					clif_skill_nodamage(target,target,SR_CRESCENTELBOW_AUTOSPELL,tsc->data[SC_CRESCENTELBOW]->val1,1);
					skill_blown(target,src,skill_get_blewcount(SR_CRESCENTELBOW_AUTOSPELL,tsc->data[SC_CRESCENTELBOW]->val1),unit_getdir(src),0);
					status_damage(NULL,target,rdamage/10,0,0,1);
					clif_damage(target, target, tick, wd.amotion, wd.dmotion, rdamage/10, wd.div_ , wd.type, wd.damage2);
					status_change_end(target, SC_CRESCENTELBOW, INVALID_TIMER);
				}
				rdelay = clif_damage(src, src, tick, wd.amotion, sstatus->dmotion, rdamage, 1, 4, 0);
				//Use Reflect Shield to signal this kind of skill trigger. [Skotlex]
				skill_additional_effect(target,src,CR_REFLECTSHIELD,1,BF_WEAPON|BF_SHORT|BF_NORMAL,ATK_DEF,tick);
			}
		}
	}

	if( tsc )
	{
		if( tsc->data[SC_DEVOTION] )
		{
			struct status_change_entry *sce = tsc->data[SC_DEVOTION];
			struct block_list *d_bl = map_id2bl(sce->val1);

			if( d_bl && (
				(d_bl->type == BL_MER && ((TBL_MER*)d_bl)->master && ((TBL_MER*)d_bl)->master->bl.id == target->id) ||
				(d_bl->type == BL_PC && ((TBL_PC*)d_bl)->devotion[sce->val2] == target->id)
				) && check_distance_bl(target, d_bl, sce->val3) )
			{
				clif_damage(d_bl, d_bl, gettick(), 0, 0, damage, 0, 0, 0);
				status_fix_damage(NULL, d_bl, damage, 0);
			}
			else
				status_change_end(target, SC_DEVOTION, INVALID_TIMER);
		}

		if( tsc->data[SC_CIRCLE_OF_FIRE_OPTION] && (wd.flag&BF_SHORT) && target->type == BL_PC )
		{
			struct elemental_data *ed = ((TBL_PC*)target)->ed;
			if( ed )
			{
				clif_skill_damage(&ed->bl, target, tick, status_get_amotion(src), 0, -30000, 1, EL_CIRCLE_OF_FIRE, tsc->data[SC_CIRCLE_OF_FIRE_OPTION]->val1, 6);
				skill_attack(BF_MAGIC,&ed->bl,&ed->bl,src,EL_CIRCLE_OF_FIRE,tsc->data[SC_CIRCLE_OF_FIRE_OPTION]->val1,tick,wd.flag);
			}
		}

		if( tsc->data[SC_WATER_SCREEN_OPTION] && tsc->data[SC_WATER_SCREEN_OPTION]->val1 )
		{
			struct block_list *e_bl = map_id2bl(tsc->data[SC_WATER_SCREEN_OPTION]->val1);
			if( e_bl && !status_isdead(e_bl) )
			{
				clif_damage(e_bl,e_bl,tick,wd.amotion,wd.dmotion,damage,wd.div_,wd.type,wd.damage2);
				status_damage(target,e_bl,damage,0,0,0);
				// Just show damage in target.
				clif_damage(src, target, tick, wd.amotion, wd.dmotion, damage, wd.div_, wd.type, wd.damage2 );
				return ATK_NONE;
			}			
		}
	}

	wd.dmotion = clif_damage(src, target, tick, wd.amotion, wd.dmotion, wd.damage, wd.div_ , wd.type, wd.damage2);

	if (sd && sd->splash_range > 0 && damage > 0)
		skill_castend_damage_id(src, target, 0, 1, tick, 0);

	map_freeblock_lock();

	battle_delay_damage(tick, wd.amotion, src, target, wd.flag, 0, 0, damage, wd.dmg_lv, wd.dmotion);

	if (sc && sc->data[SC_AUTOSPELL] && rand()%100 < sc->data[SC_AUTOSPELL]->val4) {
		int sp = 0;
		int skillid = sc->data[SC_AUTOSPELL]->val2;
		int skilllv = sc->data[SC_AUTOSPELL]->val3;
		int i = rand()%100;
		if (sc->data[SC_SPIRIT] && sc->data[SC_SPIRIT]->val2 == SL_SAGE)
			i = 0; //Max chance, no skilllv reduction. [Skotlex]
		if (i >= 50) skilllv -= 2;
		else if (i >= 15) skilllv--;
		if (skilllv < 1) skilllv = 1;
		sp = skill_get_sp(skillid,skilllv) * 2 / 3;

		if (sd) sd->state.autocast = 1;
		if (status_charge(src, 0, sp)) {
			switch (skill_get_casttype(skillid)) {
				case CAST_GROUND:
					skill_castend_pos2(src, target->x, target->y, skillid, skilllv, tick, flag);
					break;
				case CAST_NODAMAGE:
					skill_castend_nodamage_id(src, target, skillid, skilllv, tick, flag);
					break;
				case CAST_DAMAGE:
					skill_castend_damage_id(src, target, skillid, skilllv, tick, flag);
					break;
			}
		}
		if (sd) sd->state.autocast = 0;
	}
	if( sd && wd.flag&BF_SHORT && sc && sc->data[SC__AUTOSHADOWSPELL] && rand()%100 < sc->data[SC__AUTOSHADOWSPELL]->val3 &&
		sd->status.skill[sc->data[SC__AUTOSHADOWSPELL]->val1].id != 0 && sd->status.skill[sc->data[SC__AUTOSHADOWSPELL]->val1].flag == 13 )
	{
		int r_skill = sd->status.skill[sc->data[SC__AUTOSHADOWSPELL]->val1].id,
			r_lv = sc->data[SC__AUTOSHADOWSPELL]->val2;

			skill_consume_requirement(sd,r_skill,r_lv,3);
			switch( skill_get_casttype(r_skill) )
			{
			case CAST_GROUND:
				skill_castend_pos2(src, target->x, target->y, r_skill, r_lv, tick, flag);
				break;
			case CAST_DAMAGE:
				skill_castend_damage_id(src, target, r_skill, r_lv, tick, flag);
				break;
			case CAST_NODAMAGE:
				skill_castend_nodamage_id(src, target, r_skill, r_lv, tick, flag);
				break;
			}

			sd->ud.canact_tick = tick + skill_delayfix(src, r_skill, r_lv);
			clif_status_change(src, SI_ACTIONDELAY, 1, skill_delayfix(src, r_skill, r_lv), 0, 0, 1);
	}
	if( sd && sc && (sc->data[SC_TROPIC_OPTION] || sc->data[SC_CHILLY_AIR_OPTION] || sc->data[SC_WILD_STORM_OPTION] || sc->data[SC_UPHEAVAL_OPTION]) )
	{	// Autocast one Bolt depending on status change.
		int skillid = 0;
		if( sc->data[SC_TROPIC_OPTION] ) skillid = sc->data[SC_TROPIC_OPTION]->val3;
		else if( sc->data[SC_CHILLY_AIR_OPTION] ) skillid = sc->data[SC_CHILLY_AIR_OPTION]->val3;
		else if( sc->data[SC_WILD_STORM_OPTION] ) skillid = sc->data[SC_WILD_STORM_OPTION]->val2;
		else if( sc->data[SC_UPHEAVAL_OPTION] ) skillid = sc->data[SC_UPHEAVAL_OPTION]->val2;

		sd->state.autocast = 1;
		if( skillid && rand()%100 < 5 )
			skill_castend_damage_id(src, target, skillid, pc_checkskill(sd,skillid), tick, flag);
		sd->state.autocast = 0;
	}

	if (sd) {
		if (wd.flag & BF_WEAPON && src != target && damage > 0) {
			if (battle_config.left_cardfix_to_right)
				battle_drain(sd, target, wd.damage, wd.damage, tstatus->race, is_boss(target));
			else
				battle_drain(sd, target, wd.damage, wd.damage2, tstatus->race, is_boss(target));
		}
	}
	if( rdamage > 0 && !(tsc && tsc->data[SC_REFLECTDAMAGE]) ) { //By sending attack type "none" skill_additional_effect won't be invoked. [Skotlex]
		if(tsd && src != target)
			battle_drain(tsd, src, rdamage, rdamage, sstatus->race, is_boss(src));
		battle_delay_damage(tick, wd.amotion, target, src, 0, 0, 0, rdamage, ATK_DEF, rdelay);
	}

	if (tsc) {
		if (tsc->data[SC_POISONREACT] && 
			(rand()%100 < tsc->data[SC_POISONREACT]->val3
			|| sstatus->def_ele == ELE_POISON) &&
//			check_distance_bl(src, target, tstatus->rhw.range+1) && Doesn't checks range! o.O;
			status_check_skilluse(target, src, TF_POISON, tsc->data[SC_POISONREACT]->val1, 0)
		) {	//Poison React
			struct status_change_entry *sce = tsc->data[SC_POISONREACT];
			if (sstatus->def_ele == ELE_POISON) {
				sce->val2 = 0;
				skill_attack(BF_WEAPON,target,target,src,AS_POISONREACT,sce->val1,tick,0);
			} else {
				skill_attack(BF_WEAPON,target,target,src,TF_POISON, 5, tick, 0);
				--sce->val2;
			}
			if (sce->val2 <= 0)
				status_change_end(target, SC_POISONREACT, INVALID_TIMER);
		}
	}
	map_freeblock_unlock();
	return wd.dmg_lv;
}

int battle_check_undead(int race,int element)
{
	if(battle_config.undead_detect_type == 0) {
		if(element == ELE_UNDEAD)
			return 1;
	}
	else if(battle_config.undead_detect_type == 1) {
		if(race == RC_UNDEAD)
			return 1;
	}
	else {
		if(element == ELE_UNDEAD || race == RC_UNDEAD)
			return 1;
	}
	return 0;
}

//Returns the upmost level master starting with the given object
struct block_list* battle_get_master(struct block_list *src)
{
	struct block_list *prev; //Used for infinite loop check (master of yourself?)
	do {
		prev = src;
		switch (src->type) {
			case BL_PET:
				if (((TBL_PET*)src)->msd)
					src = (struct block_list*)((TBL_PET*)src)->msd;
				break;
			case BL_MOB:
				if (((TBL_MOB*)src)->master_id)
					src = map_id2bl(((TBL_MOB*)src)->master_id);
				break;
			case BL_HOM:
				if (((TBL_HOM*)src)->master)
					src = (struct block_list*)((TBL_HOM*)src)->master;
				break;
			case BL_MER:
				if (((TBL_MER*)src)->master)
					src = (struct block_list*)((TBL_MER*)src)->master;
				break;
			case BL_ELEM:
				if (((TBL_ELEM*)src)->master)
					src = (struct block_list*)((TBL_ELEM*)src)->master;
				break;
			case BL_SKILL:
				if (((TBL_SKILL*)src)->group && ((TBL_SKILL*)src)->group->src_id)
					src = map_id2bl(((TBL_SKILL*)src)->group->src_id);
				break;
		}
	} while (src && src != prev);
	return prev;
}

/*==========================================
 * Checks the state between two targets (rewritten by Skotlex)
 * (enemy, friend, party, guild, etc)
 * See battle.h for possible values/combinations
 * to be used here (BCT_* constants)
 * Return value is:
 * 1: flag holds true (is enemy, party, etc)
 * -1: flag fails
 * 0: Invalid target (non-targetable ever)
 *------------------------------------------*/
int battle_check_target( struct block_list *src, struct block_list *target,int flag)
{
	int m,state = 0; //Initial state none
	int strip_enemy = 1; //Flag which marks whether to remove the BCT_ENEMY status if it's also friend/ally.
	struct block_list *s_bl = src, *t_bl = target;

	nullpo_ret(src);
	nullpo_ret(target);

	m = target->m;

	//t_bl/s_bl hold the 'master' of the attack, while src/target are the actual
	//objects involved.
	if( (t_bl = battle_get_master(target)) == NULL )
		t_bl = target;

	if( (s_bl = battle_get_master(src)) == NULL )
		s_bl = src;

	switch( target->type )
	{ // Checks on actual target
		case BL_PC:
			if (((TBL_PC*)target)->invincible_timer != INVALID_TIMER || pc_isinvisible((TBL_PC*)target))
				return -1; //Cannot be targeted yet.
			break;
		case BL_MOB:
			if((((TBL_MOB*)target)->special_state.ai == 2 || //Marine Spheres
				(((TBL_MOB*)target)->special_state.ai == 3 && battle_config.summon_flora&1)) && //Floras
				s_bl->type == BL_PC && src->type != BL_MOB)
			{	//Targettable by players
				state |= BCT_ENEMY;
				strip_enemy = 0;
			}
			break;
		case BL_SKILL:
		{
			TBL_SKILL *su = (TBL_SKILL*)target;
			if( !su->group )
				return 0;
			if( skill_get_inf2(su->group->skill_id)&INF2_TRAP )
			{ //Only a few skills can target traps...
				switch( battle_getcurrentskill(src) )
				{
					case MA_REMOVETRAP:
					case HT_REMOVETRAP:
					case AC_SHOWER:
					case MA_SHOWER:
					case WZ_SIGHTRASHER:
					case WZ_SIGHTBLASTER:
					case SM_MAGNUM:
					case MS_MAGNUM:
					case RA_DETONATOR:
					case RA_SENSITIVEKEEN:
						state |= BCT_ENEMY;
						strip_enemy = 0;
						break;
					default:
						return 0;
				}
			} else if (su->group->skill_id==WZ_ICEWALL || su->group->skill_id == GN_WALLOFTHORN)
			{
				state |= BCT_ENEMY;
				strip_enemy = 0;
			} else	//Excepting traps and icewall, you should not be able to target skills.
				return 0;
		}
			break;
		//Valid targets with no special checks here.
		case BL_MER:
		case BL_HOM:
		case BL_ELEM:
			break;
		//All else not specified is an invalid target.
		default:	
			return 0;
	}

	switch( t_bl->type )
	{	//Checks on target master
		case BL_PC:
		{
			struct map_session_data *sd;
			if( t_bl == s_bl ) break;
			sd = BL_CAST(BL_PC, t_bl);

			if( sd->state.monster_ignore && flag&BCT_ENEMY )
				return 0; // Global inminuty only to Attacks
			if( sd->status.karma && s_bl->type == BL_PC && ((TBL_PC*)s_bl)->status.karma )
				state |= BCT_ENEMY; // Characters with bad karma may fight amongst them
			if( sd->state.killable ) {
				state |= BCT_ENEMY; // Everything can kill it
				strip_enemy = 0;
			}
			break;
		}
		case BL_MOB:
		{
			struct mob_data *md = BL_CAST(BL_MOB, t_bl);

			if( !((agit_flag || agit2_flag) && map[m].flag.gvg_castle) && md->guardian_data && md->guardian_data->guild_id )
				return 0; // Disable guardians/emperiums owned by Guilds on non-woe times.
			break;
		}
	}

	switch( src->type )
  	{	//Checks on actual src type
		case BL_PET:
			if (t_bl->type != BL_MOB && flag&BCT_ENEMY)
				return 0; //Pet may not attack non-mobs.
			if (t_bl->type == BL_MOB && ((TBL_MOB*)t_bl)->guardian_data && flag&BCT_ENEMY)
				return 0; //pet may not attack Guardians/Emperium
			break;
		case BL_SKILL:
		{
			struct skill_unit *su = (struct skill_unit *)src;
			if (!su->group)
				return 0;

			if (su->group->src_id == target->id)
			{
				int inf2;
				inf2 = skill_get_inf2(su->group->skill_id);
				if (inf2&INF2_NO_TARGET_SELF)
					return -1;
				if (inf2&INF2_TARGET_SELF)
					return 1;
			}
			break;
		}
	}

	switch( s_bl->type )
	{	//Checks on source master
		case BL_PC:
		{
			struct map_session_data *sd = BL_CAST(BL_PC, s_bl);
			if( s_bl != t_bl )
			{
				if( sd->state.killer )
				{
					state |= BCT_ENEMY; // Can kill anything
					strip_enemy = 0;
				}
				else if( sd->duel_group && !((!battle_config.duel_allow_pvp && map[m].flag.pvp) || (!battle_config.duel_allow_gvg && map_flag_gvg(m))) )
		  		{
					if( t_bl->type == BL_PC && (sd->duel_group == ((TBL_PC*)t_bl)->duel_group) )
						return (BCT_ENEMY&flag)?1:-1; // Duel targets can ONLY be your enemy, nothing else.
					else
						return 0; // You can't target anything out of your duel
				}
			}
			if( map_flag_gvg(m) && !sd->status.guild_id && t_bl->type == BL_MOB && ((TBL_MOB*)t_bl)->guardian_data )
				return 0; //If you don't belong to a guild, can't target guardians/emperium.
			if( t_bl->type != BL_PC )
				state |= BCT_ENEMY; //Natural enemy.
			break;
		}
		case BL_MOB:
		{
			struct mob_data *md = BL_CAST(BL_MOB, s_bl);
			if( !((agit_flag || agit2_flag) && map[m].flag.gvg_castle) && md->guardian_data && md->guardian_data->guild_id )
				return 0; // Disable guardians/emperium owned by Guilds on non-woe times.

			if( !md->special_state.ai )
			{ //Normal mobs.
				if( t_bl->type == BL_MOB && !((TBL_MOB*)t_bl)->special_state.ai )
					state |= BCT_PARTY; //Normal mobs with no ai are friends.
				else
					state |= BCT_ENEMY; //However, all else are enemies.
			}
			else
			{
				if( t_bl->type == BL_MOB && !((TBL_MOB*)t_bl)->special_state.ai )
					state |= BCT_ENEMY; //Natural enemy for AI mobs are normal mobs.
			}
			break;
		}
		default:
		//Need some sort of default behaviour for unhandled types.
			if (t_bl->type != s_bl->type)
				state |= BCT_ENEMY;
			break;
	}
	
	if( (flag&BCT_ALL) == BCT_ALL )
	{ //All actually stands for all attackable chars 
		if( target->type&BL_CHAR )
			return 1;
		else
			return -1;
	}
	if( flag == BCT_NOONE ) //Why would someone use this? no clue.
		return -1;
	
	if( t_bl == s_bl )
	{ //No need for further testing.
		state |= BCT_SELF|BCT_PARTY|BCT_GUILD;
		if( state&BCT_ENEMY && strip_enemy )
			state&=~BCT_ENEMY;
		return (flag&state)?1:-1;
	}
	
	if( map_flag_vs(m) )
	{ //Check rivalry settings.
		int sbg_id = 0, tbg_id = 0;
		if( map[m].flag.battleground )
		{
			sbg_id = bg_team_get_id(s_bl);
			tbg_id = bg_team_get_id(t_bl);
		}
		if( flag&(BCT_PARTY|BCT_ENEMY) )
		{
			int s_party = status_get_party_id(s_bl);
			if( s_party && s_party == status_get_party_id(t_bl) && !(map[m].flag.pvp && map[m].flag.pvp_noparty) && !(map_flag_gvg(m) && map[m].flag.gvg_noparty) && (!map[m].flag.battleground || sbg_id == tbg_id) )
				state |= BCT_PARTY;
			else
				state |= BCT_ENEMY;
		}
		if( flag&(BCT_GUILD|BCT_ENEMY) )
		{
			int s_guild = status_get_guild_id(s_bl);
			int t_guild = status_get_guild_id(t_bl);
			if( !(map[m].flag.pvp && map[m].flag.pvp_noguild) && s_guild && t_guild && (s_guild == t_guild || guild_isallied(s_guild, t_guild)) && (!map[m].flag.battleground || sbg_id == tbg_id) )
				state |= BCT_GUILD;
			else
				state |= BCT_ENEMY;
		}
		if( state&BCT_ENEMY && map[m].flag.battleground && sbg_id && sbg_id == tbg_id )
			state &= ~BCT_ENEMY;

		if( state&BCT_ENEMY && battle_config.pk_mode && !map_flag_gvg(m) && s_bl->type == BL_PC && t_bl->type == BL_PC )
		{ // Prevent novice engagement on pk_mode (feature by Valaris)
			TBL_PC *sd = (TBL_PC*)s_bl, *sd2 = (TBL_PC*)t_bl;
			if (
				(sd->class_&MAPID_UPPERMASK) == MAPID_NOVICE ||
				(sd2->class_&MAPID_UPPERMASK) == MAPID_NOVICE ||
				(int)sd->status.base_level < battle_config.pk_min_level ||
			  	(int)sd2->status.base_level < battle_config.pk_min_level ||
				(battle_config.pk_level_range && abs((int)sd->status.base_level - (int)sd2->status.base_level) > battle_config.pk_level_range)
			)
				state &= ~BCT_ENEMY;
		}
	}
	else
	{ //Non pvp/gvg, check party/guild settings.
		if( flag&BCT_PARTY || state&BCT_ENEMY )
		{
			int s_party = status_get_party_id(s_bl);
			if(s_party && s_party == status_get_party_id(t_bl))
				state |= BCT_PARTY;
		}
		if( flag&BCT_GUILD || state&BCT_ENEMY )
		{
			int s_guild = status_get_guild_id(s_bl);
			int t_guild = status_get_guild_id(t_bl);
			if(s_guild && t_guild && (s_guild == t_guild || guild_isallied(s_guild, t_guild)))
				state |= BCT_GUILD;
		}
	}
	
	if( !state ) //If not an enemy, nor a guild, nor party, nor yourself, it's neutral.
		state = BCT_NEUTRAL;
	//Alliance state takes precedence over enemy one.
	else if( state&BCT_ENEMY && strip_enemy && state&(BCT_SELF|BCT_PARTY|BCT_GUILD) )
		state&=~BCT_ENEMY;

	return (flag&state)?1:-1;
}
/*==========================================
 * �˒�����
 *------------------------------------------*/
bool battle_check_range(struct block_list *src, struct block_list *bl, int range)
{
	int d;
	nullpo_retr(false, src);
	nullpo_retr(false, bl);

	if( src->m != bl->m )
		return false;

#ifndef CIRCULAR_AREA
	if( src->type == BL_PC )
	{ // Range for players' attacks and skills should always have a circular check. [Inkfish]
		int dx = src->x - bl->x, dy = src->y - bl->y;
		if( !check_distance(dx*dx + dy*dy, 0, range*range+(dx&&dy?1:0)) )
			return false;
	}
	else
#endif
	if( !check_distance_bl(src, bl, range) )
		return false;

	if( (d = distance_bl(src, bl)) < 2 )
		return true;  // No need for path checking.

	if( d > AREA_SIZE )
		return false; // Avoid targetting objects beyond your range of sight.

	return path_search_long(NULL,src->m,src->x,src->y,bl->x,bl->y,CELL_CHKWALL);
}

static const struct _battle_data {
	const char* str;
	int* val;
	int defval;
	int min;
	int max;
} battle_data[] = {
	{ "warp_point_debug",                   &battle_config.warp_point_debug,                0,      0,      1,              },
	{ "enable_critical",                    &battle_config.enable_critical,                 BL_PC,  BL_NUL, BL_ALL,         },
	{ "mob_critical_rate",                  &battle_config.mob_critical_rate,               100,    0,      INT_MAX,        },
	{ "critical_rate",                      &battle_config.critical_rate,                   100,    0,      INT_MAX,        },
	{ "enable_baseatk",                     &battle_config.enable_baseatk,                  BL_PC|BL_HOM, BL_NUL, BL_ALL,   },
	{ "enable_perfect_flee",                &battle_config.enable_perfect_flee,             BL_PC|BL_PET, BL_NUL, BL_ALL,   },
	{ "casting_rate",                       &battle_config.cast_rate,                       100,    0,      INT_MAX,        },
	{ "delay_rate",                         &battle_config.delay_rate,                      100,    0,      INT_MAX,        },
	{ "delay_dependon_dex",                 &battle_config.delay_dependon_dex,              0,      0,      1,              },
	{ "delay_dependon_agi",                 &battle_config.delay_dependon_agi,              0,      0,      1,              },
	{ "skill_delay_attack_enable",          &battle_config.sdelay_attack_enable,            0,      0,      1,              },
	{ "left_cardfix_to_right",              &battle_config.left_cardfix_to_right,           0,      0,      1,              },
	{ "skill_add_range",                    &battle_config.skill_add_range,                 0,      0,      INT_MAX,        },
	{ "skill_out_range_consume",            &battle_config.skill_out_range_consume,         1,      0,      1,              },
	{ "skillrange_by_distance",             &battle_config.skillrange_by_distance,          ~BL_PC, BL_NUL, BL_ALL,         },
	{ "skillrange_from_weapon",             &battle_config.use_weapon_skill_range,          ~BL_PC, BL_NUL, BL_ALL,         },
	{ "player_damage_delay_rate",           &battle_config.pc_damage_delay_rate,            100,    0,      INT_MAX,        },
	{ "defunit_not_enemy",                  &battle_config.defnotenemy,                     0,      0,      1,              },
	{ "gvg_traps_target_all",               &battle_config.vs_traps_bctall,                 BL_PC,  BL_NUL, BL_ALL,         },
	{ "traps_setting",                      &battle_config.traps_setting,                   0,      0,      1,              },
	{ "summon_flora_setting",               &battle_config.summon_flora,                    1|2,    0,      1|2,            },
	{ "clear_skills_on_death",              &battle_config.clear_unit_ondeath,              BL_NUL, BL_NUL, BL_ALL,         },
	{ "clear_skills_on_warp",               &battle_config.clear_unit_onwarp,               BL_ALL, BL_NUL, BL_ALL,         },
	{ "random_monster_checklv",             &battle_config.random_monster_checklv,          0,      0,      1,              },
	{ "attribute_recover",                  &battle_config.attr_recover,                    1,      0,      1,              },
	{ "flooritem_lifetime",                 &battle_config.flooritem_lifetime,              60000,  1000,   INT_MAX,        },
	{ "item_auto_get",                      &battle_config.item_auto_get,                   0,      0,      1,              },
	{ "item_first_get_time",                &battle_config.item_first_get_time,             3000,   0,      INT_MAX,        },
	{ "item_second_get_time",               &battle_config.item_second_get_time,            1000,   0,      INT_MAX,        },
	{ "item_third_get_time",                &battle_config.item_third_get_time,             1000,   0,      INT_MAX,        },
	{ "mvp_item_first_get_time",            &battle_config.mvp_item_first_get_time,         10000,  0,      INT_MAX,        },
	{ "mvp_item_second_get_time",           &battle_config.mvp_item_second_get_time,        10000,  0,      INT_MAX,        },
	{ "mvp_item_third_get_time",            &battle_config.mvp_item_third_get_time,         2000,   0,      INT_MAX,        },
	{ "drop_rate0item",                     &battle_config.drop_rate0item,                  0,      0,      1,              },
	{ "base_exp_rate",                      &battle_config.base_exp_rate,                   100,    0,      INT_MAX,        },
	{ "job_exp_rate",                       &battle_config.job_exp_rate,                    100,    0,      INT_MAX,        },
	{ "pvp_exp",                            &battle_config.pvp_exp,                         1,      0,      1,              },
	{ "death_penalty_type",                 &battle_config.death_penalty_type,              0,      0,      2,              },
	{ "death_penalty_base",                 &battle_config.death_penalty_base,              0,      0,      INT_MAX,        },
	{ "death_penalty_job",                  &battle_config.death_penalty_job,               0,      0,      INT_MAX,        },
	{ "zeny_penalty",                       &battle_config.zeny_penalty,                    0,      0,      INT_MAX,        },
	{ "hp_rate",                            &battle_config.hp_rate,                         100,    1,      INT_MAX,        },
	{ "sp_rate",                            &battle_config.sp_rate,                         100,    1,      INT_MAX,        },
	{ "restart_hp_rate",                    &battle_config.restart_hp_rate,                 0,      0,      100,            },
	{ "restart_sp_rate",                    &battle_config.restart_sp_rate,                 0,      0,      100,            },
	{ "guild_aura",                         &battle_config.guild_aura,                      31,     0,      31,             },
	{ "mvp_hp_rate",                        &battle_config.mvp_hp_rate,                     100,    1,      INT_MAX,        },
	{ "mvp_exp_rate",                       &battle_config.mvp_exp_rate,                    100,    0,      INT_MAX,        },
	{ "monster_hp_rate",                    &battle_config.monster_hp_rate,                 100,    1,      INT_MAX,        },
	{ "monster_max_aspd",                   &battle_config.monster_max_aspd,                199,    100,    199,            },
	{ "view_range_rate",                    &battle_config.view_range_rate,                 100,    0,      INT_MAX,        },
	{ "chase_range_rate",                   &battle_config.chase_range_rate,                100,    0,      INT_MAX,        },
	{ "gtb_sc_immunity",                    &battle_config.gtb_sc_immunity,                 50,     0,      INT_MAX,        },
	{ "guild_max_castles",                  &battle_config.guild_max_castles,               0,      0,      INT_MAX,        },
	{ "guild_skill_relog_delay",            &battle_config.guild_skill_relog_delay,         0,      0,      1,              },
	{ "emergency_call",                     &battle_config.emergency_call,                  11,     0,      31,             },
	{ "lowest_gm_level",                    &battle_config.lowest_gm_level,                 1,      0,      99,             },
	{ "atcommand_gm_only",                  &battle_config.atc_gmonly,                      0,      0,      1,              },
	{ "atcommand_spawn_quantity_limit",     &battle_config.atc_spawn_quantity_limit,        100,    0,      INT_MAX,        },
	{ "atcommand_slave_clone_limit",        &battle_config.atc_slave_clone_limit,           25,     0,      INT_MAX,        },
	{ "partial_name_scan",                  &battle_config.partial_name_scan,               0,      0,      1,              },
	{ "gm_all_skill",                       &battle_config.gm_allskill,                     0,      0,      100,            },
	{ "gm_all_equipment",                   &battle_config.gm_allequip,                     0,      0,      100,            },
	{ "gm_skill_unconditional",             &battle_config.gm_skilluncond,                  0,      0,      100,            },
	{ "gm_join_chat",                       &battle_config.gm_join_chat,                    0,      0,      100,            },
	{ "gm_kick_chat",                       &battle_config.gm_kick_chat,                    0,      0,      100,            },
	{ "gm_can_party",                       &battle_config.gm_can_party,                    0,      0,      1,              },
	{ "gm_cant_party_min_lv",               &battle_config.gm_cant_party_min_lv,            20,     0,      100,            },
	{ "player_skillfree",                   &battle_config.skillfree,                       0,      0,      1,              },
	{ "player_skillup_limit",               &battle_config.skillup_limit,                   1,      0,      1,              },
	{ "weapon_produce_rate",                &battle_config.wp_rate,                         100,    0,      INT_MAX,        },
	{ "potion_produce_rate",                &battle_config.pp_rate,                         100,    0,      INT_MAX,        },
	{ "monster_active_enable",              &battle_config.monster_active_enable,           1,      0,      1,              },
	{ "monster_damage_delay_rate",          &battle_config.monster_damage_delay_rate,       100,    0,      INT_MAX,        },
	{ "monster_loot_type",                  &battle_config.monster_loot_type,               0,      0,      1,              },
//	{ "mob_skill_use",                      &battle_config.mob_skill_use,                   1,      0,      1,              }, //Deprecated
	{ "mob_skill_rate",                     &battle_config.mob_skill_rate,                  100,    0,      INT_MAX,        },
	{ "mob_skill_delay",                    &battle_config.mob_skill_delay,                 100,    0,      INT_MAX,        },
	{ "mob_count_rate",                     &battle_config.mob_count_rate,                  100,    0,      INT_MAX,        },
	{ "mob_spawn_delay",                    &battle_config.mob_spawn_delay,                 100,    0,      INT_MAX,        },
	{ "plant_spawn_delay",                  &battle_config.plant_spawn_delay,               100,    0,      INT_MAX,        },
	{ "boss_spawn_delay",                   &battle_config.boss_spawn_delay,                100,    0,      INT_MAX,        },
	{ "no_spawn_on_player",                 &battle_config.no_spawn_on_player,              0,      0,      100,            },
	{ "force_random_spawn",                 &battle_config.force_random_spawn,              0,      0,      1,              },
	{ "slaves_inherit_mode",                &battle_config.slaves_inherit_mode,             2,      0,      3,              },
	{ "slaves_inherit_speed",               &battle_config.slaves_inherit_speed,            3,      0,      3,              },
	{ "summons_trigger_autospells",         &battle_config.summons_trigger_autospells,      1,      0,      1,              },
	{ "pc_damage_walk_delay_rate",          &battle_config.pc_walk_delay_rate,              20,     0,      INT_MAX,        },
	{ "damage_walk_delay_rate",             &battle_config.walk_delay_rate,                 100,    0,      INT_MAX,        },
	{ "multihit_delay",                     &battle_config.multihit_delay,                  80,     0,      INT_MAX,        },
	{ "quest_skill_learn",                  &battle_config.quest_skill_learn,               0,      0,      1,              },
	{ "quest_skill_reset",                  &battle_config.quest_skill_reset,               0,      0,      1,              },
	{ "basic_skill_check",                  &battle_config.basic_skill_check,               1,      0,      1,              },
	{ "guild_emperium_check",               &battle_config.guild_emperium_check,            1,      0,      1,              },
	{ "guild_exp_limit",                    &battle_config.guild_exp_limit,                 50,     0,      99,             },
	{ "player_invincible_time",             &battle_config.pc_invincible_time,              5000,   0,      INT_MAX,        },
	{ "pet_catch_rate",                     &battle_config.pet_catch_rate,                  100,    0,      INT_MAX,        },
	{ "pet_rename",                         &battle_config.pet_rename,                      0,      0,      1,              },
	{ "pet_friendly_rate",                  &battle_config.pet_friendly_rate,               100,    0,      INT_MAX,        },
	{ "pet_hungry_delay_rate",              &battle_config.pet_hungry_delay_rate,           100,    10,     INT_MAX,        },
	{ "pet_hungry_friendly_decrease",       &battle_config.pet_hungry_friendly_decrease,    5,      0,      INT_MAX,        },
	{ "pet_status_support",                 &battle_config.pet_status_support,              0,      0,      1,              },
	{ "pet_attack_support",                 &battle_config.pet_attack_support,              0,      0,      1,              },
	{ "pet_damage_support",                 &battle_config.pet_damage_support,              0,      0,      1,              },
	{ "pet_support_min_friendly",           &battle_config.pet_support_min_friendly,        900,    0,      950,            },
	{ "pet_equip_min_friendly",             &battle_config.pet_equip_min_friendly,          900,    0,      950,            },
	{ "pet_support_rate",                   &battle_config.pet_support_rate,                100,    0,      INT_MAX,        },
	{ "pet_attack_exp_to_master",           &battle_config.pet_attack_exp_to_master,        0,      0,      1,              },
	{ "pet_attack_exp_rate",                &battle_config.pet_attack_exp_rate,             100,    0,      INT_MAX,        },
	{ "pet_lv_rate",                        &battle_config.pet_lv_rate,                     0,      0,      INT_MAX,        },
	{ "pet_max_stats",                      &battle_config.pet_max_stats,                   99,     0,      INT_MAX,        },
	{ "pet_max_atk1",                       &battle_config.pet_max_atk1,                    750,    0,      INT_MAX,        },
	{ "pet_max_atk2",                       &battle_config.pet_max_atk2,                    1000,   0,      INT_MAX,        },
	{ "pet_disable_in_gvg",                 &battle_config.pet_no_gvg,                      0,      0,      1,              },
	{ "skill_min_damage",                   &battle_config.skill_min_damage,                2|4,    0,      1|2|4,          },
	{ "finger_offensive_type",              &battle_config.finger_offensive_type,           0,      0,      1,              },
	{ "heal_exp",                           &battle_config.heal_exp,                        0,      0,      INT_MAX,        },
	{ "resurrection_exp",                   &battle_config.resurrection_exp,                0,      0,      INT_MAX,        },
	{ "shop_exp",                           &battle_config.shop_exp,                        0,      0,      INT_MAX,        },
	{ "max_heal_lv",                        &battle_config.max_heal_lv,                     11,     1,      INT_MAX,        },
	{ "max_heal",                           &battle_config.max_heal,                        9999,   0,      INT_MAX,        },
	{ "combo_delay_rate",                   &battle_config.combo_delay_rate,                100,    0,      INT_MAX,        },
	{ "item_check",                         &battle_config.item_check,                      0,      0,      1,              },
	{ "item_use_interval",                  &battle_config.item_use_interval,               100,    0,      INT_MAX,        },
	{ "cashfood_use_interval",              &battle_config.cashfood_use_interval,           60000,  0,      INT_MAX,        },
	{ "wedding_modifydisplay",              &battle_config.wedding_modifydisplay,           0,      0,      1,              },
	{ "wedding_ignorepalette",              &battle_config.wedding_ignorepalette,           0,      0,      1,              },
	{ "xmas_ignorepalette",                 &battle_config.xmas_ignorepalette,              0,      0,      1,              },
	{ "summer_ignorepalette",               &battle_config.summer_ignorepalette,            0,      0,      1,              },
	{ "natural_healhp_interval",            &battle_config.natural_healhp_interval,         6000,   NATURAL_HEAL_INTERVAL, INT_MAX, },
	{ "natural_healsp_interval",            &battle_config.natural_healsp_interval,         8000,   NATURAL_HEAL_INTERVAL, INT_MAX, },
	{ "natural_heal_skill_interval",        &battle_config.natural_heal_skill_interval,     10000,  NATURAL_HEAL_INTERVAL, INT_MAX, },
	{ "natural_heal_weight_rate",           &battle_config.natural_heal_weight_rate,        50,     50,     101             },
	{ "arrow_decrement",                    &battle_config.arrow_decrement,                 1,      0,      2,              },
	{ "max_aspd",                           &battle_config.max_aspd,                        199,    100,    199,            },
	{ "max_walk_speed",                     &battle_config.max_walk_speed,                  300,    100,    100*DEFAULT_WALK_SPEED, },
	{ "max_lv",                             &battle_config.max_lv,                          99,     0,      127,            },
	{ "aura_lv",                            &battle_config.aura_lv,                         99,     0,      INT_MAX,        },
	{ "max_hp",                             &battle_config.max_hp,                          32500,  100,    1000000000,     },
	{ "max_sp",                             &battle_config.max_sp,                          32500,  100,    1000000000,     },
	{ "max_cart_weight",                    &battle_config.max_cart_weight,                 8000,   100,    1000000,        },
	{ "max_parameter",                      &battle_config.max_parameter,                   99,     10,     10000,          },
	{ "max_baby_parameter",                 &battle_config.max_baby_parameter,              80,     10,     10000,          },
	{ "max_def",                            &battle_config.max_def,                         99,     0,      INT_MAX,        },
	{ "over_def_bonus",                     &battle_config.over_def_bonus,                  0,      0,      1000,           },
	{ "skill_log",                          &battle_config.skill_log,                       BL_NUL, BL_NUL, BL_ALL,         },
	{ "battle_log",                         &battle_config.battle_log,                      0,      0,      1,              },
	{ "save_log",                           &battle_config.save_log,                        0,      0,      1,              },
	{ "etc_log",                            &battle_config.etc_log,                         1,      0,      1,              },
	{ "save_clothcolor",                    &battle_config.save_clothcolor,                 1,      0,      1,              },
	{ "undead_detect_type",                 &battle_config.undead_detect_type,              0,      0,      2,              },
	{ "auto_counter_type",                  &battle_config.auto_counter_type,               BL_ALL, BL_NUL, BL_ALL,         },
	{ "min_hitrate",                        &battle_config.min_hitrate,                     5,      0,      100,            },
	{ "max_hitrate",                        &battle_config.max_hitrate,                     100,    0,      100,            },
	{ "agi_penalty_target",                 &battle_config.agi_penalty_target,              BL_PC,  BL_NUL, BL_ALL,         },
	{ "agi_penalty_type",                   &battle_config.agi_penalty_type,                1,      0,      2,              },
	{ "agi_penalty_count",                  &battle_config.agi_penalty_count,               3,      2,      INT_MAX,        },
	{ "agi_penalty_num",                    &battle_config.agi_penalty_num,                 10,     0,      INT_MAX,        },
	{ "agi_penalty_count_lv",               &battle_config.agi_penalty_count_lv,            ATK_FLEE, 0,    INT_MAX,        },
	{ "vit_penalty_target",                 &battle_config.vit_penalty_target,              BL_PC,  BL_NUL, BL_ALL,         },
	{ "vit_penalty_type",                   &battle_config.vit_penalty_type,                1,      0,      2,              },
	{ "vit_penalty_count",                  &battle_config.vit_penalty_count,               3,      2,      INT_MAX,        },
	{ "vit_penalty_num",                    &battle_config.vit_penalty_num,                 5,      0,      INT_MAX,        },
	{ "vit_penalty_count_lv",               &battle_config.vit_penalty_count_lv,            ATK_MISS, 0,    INT_MAX,        },
	{ "weapon_defense_type",                &battle_config.weapon_defense_type,             0,      0,      INT_MAX,        },
	{ "magic_defense_type",                 &battle_config.magic_defense_type,              0,      0,      INT_MAX,        },
	{ "skill_reiteration",                  &battle_config.skill_reiteration,               BL_NUL, BL_NUL, BL_ALL,         },
	{ "skill_nofootset",                    &battle_config.skill_nofootset,                 BL_PC,  BL_NUL, BL_ALL,         },
	{ "player_cloak_check_type",            &battle_config.pc_cloak_check_type,             1,      0,      1|2|4,          },
	{ "monster_cloak_check_type",           &battle_config.monster_cloak_check_type,        4,      0,      1|2|4,          },
	{ "sense_type",                         &battle_config.estimation_type,                 1|2,    0,      1|2,            },
	{ "gvg_eliminate_time",                 &battle_config.gvg_eliminate_time,              7000,   0,      INT_MAX,        },
	{ "gvg_short_attack_damage_rate",       &battle_config.gvg_short_damage_rate,           80,     0,      INT_MAX,        },
	{ "gvg_long_attack_damage_rate",        &battle_config.gvg_long_damage_rate,            80,     0,      INT_MAX,        },
	{ "gvg_weapon_attack_damage_rate",      &battle_config.gvg_weapon_damage_rate,          60,     0,      INT_MAX,        },
	{ "gvg_magic_attack_damage_rate",       &battle_config.gvg_magic_damage_rate,           60,     0,      INT_MAX,        },
	{ "gvg_misc_attack_damage_rate",        &battle_config.gvg_misc_damage_rate,            60,     0,      INT_MAX,        },
	{ "gvg_flee_penalty",                   &battle_config.gvg_flee_penalty,                20,     0,      INT_MAX,        },
	{ "pk_short_attack_damage_rate",        &battle_config.pk_short_damage_rate,            80,     0,      INT_MAX,        },
	{ "pk_long_attack_damage_rate",         &battle_config.pk_long_damage_rate,             70,     0,      INT_MAX,        },
	{ "pk_weapon_attack_damage_rate",       &battle_config.pk_weapon_damage_rate,           60,     0,      INT_MAX,        },
	{ "pk_magic_attack_damage_rate",        &battle_config.pk_magic_damage_rate,            60,     0,      INT_MAX,        },
	{ "pk_misc_attack_damage_rate",         &battle_config.pk_misc_damage_rate,             60,     0,      INT_MAX,        },
	{ "mob_changetarget_byskill",           &battle_config.mob_changetarget_byskill,        0,      0,      1,              },
	{ "attack_direction_change",            &battle_config.attack_direction_change,         BL_ALL, BL_NUL, BL_ALL,         },
	{ "land_skill_limit",                   &battle_config.land_skill_limit,                BL_ALL, BL_NUL, BL_ALL,         },
	{ "monster_class_change_full_recover",  &battle_config.monster_class_change_recover,    1,      0,      1,              },
	{ "produce_item_name_input",            &battle_config.produce_item_name_input,         0x1|0x2, 0,     0x9F,           },
	{ "display_skill_fail",                 &battle_config.display_skill_fail,              2,      0,      1|2|4|8,        },
	{ "chat_warpportal",                    &battle_config.chat_warpportal,                 0,      0,      1,              },
	{ "mob_warp",                           &battle_config.mob_warp,                        0,      0,      1|2|4|8,        },
	{ "dead_branch_active",                 &battle_config.dead_branch_active,              1,      0,      1,              },
	{ "vending_max_value",                  &battle_config.vending_max_value,               10000000, 1,    MAX_ZENY,       },
	{ "vending_over_max",                   &battle_config.vending_over_max,                1,      0,      1,              },
	{ "show_steal_in_same_party",           &battle_config.show_steal_in_same_party,        0,      0,      1,              },
	{ "party_hp_mode",                      &battle_config.party_hp_mode,                   0,      0,      1,              },
	{ "show_party_share_picker",            &battle_config.party_show_share_picker,         1,      0,      1,              },
	{ "show_picker.item_type",              &battle_config.show_picker_item_type,           112,    0,      INT_MAX,        },
	{ "party_update_interval",              &battle_config.party_update_interval,           1000,   100,    INT_MAX,        },
	{ "party_item_share_type",              &battle_config.party_share_type,                0,      0,      1|2|3,          },
	{ "attack_attr_none",                   &battle_config.attack_attr_none,                ~BL_PC, BL_NUL, BL_ALL,         },
	{ "gx_allhit",                          &battle_config.gx_allhit,                       0,      0,      1,              },
	{ "gx_disptype",                        &battle_config.gx_disptype,                     1,      0,      1,              },
	{ "devotion_level_difference",          &battle_config.devotion_level_difference,       10,     0,      INT_MAX,        },
	{ "player_skill_partner_check",         &battle_config.player_skill_partner_check,      1,      0,      1,              },
	{ "hide_GM_session",                    &battle_config.hide_GM_session,                 0,      0,      1,              },
	{ "invite_request_check",               &battle_config.invite_request_check,            1,      0,      1,              },
	{ "skill_removetrap_type",              &battle_config.skill_removetrap_type,           0,      0,      1,              },
	{ "disp_experience",                    &battle_config.disp_experience,                 0,      0,      1,              },
	{ "disp_zeny",                          &battle_config.disp_zeny,                       0,      0,      1,              },
	{ "castle_defense_rate",                &battle_config.castle_defense_rate,             100,    0,      100,            },
	{ "gm_cant_drop_min_lv",                &battle_config.gm_cant_drop_min_lv,             1,      0,      100,            },
	{ "gm_cant_drop_max_lv",                &battle_config.gm_cant_drop_max_lv,             0,      0,      100,            },
	{ "disp_hpmeter",                       &battle_config.disp_hpmeter,                    0,      0,      100,            },
	{ "bone_drop",                          &battle_config.bone_drop,                       0,      0,      2,              },
	{ "buyer_name",                         &battle_config.buyer_name,                      1,      0,      1,              },
	{ "skill_wall_check",                   &battle_config.skill_wall_check,                1,      0,      1,              },
	{ "cell_stack_limit",                   &battle_config.cell_stack_limit,                1,      1,      255,            },
// eAthena additions
	{ "item_logarithmic_drops",             &battle_config.logarithmic_drops,               0,      0,      1,              },
	{ "item_drop_common_min",               &battle_config.item_drop_common_min,            1,      1,      10000,          },
	{ "item_drop_common_max",               &battle_config.item_drop_common_max,            10000,  1,      10000,          },
	{ "item_drop_equip_min",                &battle_config.item_drop_equip_min,             1,      1,      10000,          },
	{ "item_drop_equip_max",                &battle_config.item_drop_equip_max,             10000,  1,      10000,          },
	{ "item_drop_card_min",                 &battle_config.item_drop_card_min,              1,      1,      10000,          },
	{ "item_drop_card_max",                 &battle_config.item_drop_card_max,              10000,  1,      10000,          },
	{ "item_drop_mvp_min",                  &battle_config.item_drop_mvp_min,               1,      1,      10000,          },
	{ "item_drop_mvp_max",                  &battle_config.item_drop_mvp_max,               10000,  1,      10000,          },
	{ "item_drop_heal_min",                 &battle_config.item_drop_heal_min,              1,      1,      10000,          },
	{ "item_drop_heal_max",                 &battle_config.item_drop_heal_max,              10000,  1,      10000,          },
	{ "item_drop_use_min",                  &battle_config.item_drop_use_min,               1,      1,      10000,          },
	{ "item_drop_use_max",                  &battle_config.item_drop_use_max,               10000,  1,      10000,          },
	{ "item_drop_add_min",                  &battle_config.item_drop_adddrop_min,           1,      1,      10000,          },
	{ "item_drop_add_max",                  &battle_config.item_drop_adddrop_max,           10000,  1,      10000,          },
	{ "item_drop_treasure_min",             &battle_config.item_drop_treasure_min,          1,      1,      10000,          },
	{ "item_drop_treasure_max",             &battle_config.item_drop_treasure_max,          10000,  1,      10000,          },
	{ "item_rate_mvp",                      &battle_config.item_rate_mvp,                   100,    0,      1000000,        },
	{ "item_rate_common",                   &battle_config.item_rate_common,                100,    0,      1000000,        },
	{ "item_rate_common_boss",              &battle_config.item_rate_common_boss,           100,    0,      1000000,        },
	{ "item_rate_equip",                    &battle_config.item_rate_equip,                 100,    0,      1000000,        },
	{ "item_rate_equip_boss",               &battle_config.item_rate_equip_boss,            100,    0,      1000000,        },
	{ "item_rate_card",                     &battle_config.item_rate_card,                  100,    0,      1000000,        },
	{ "item_rate_card_boss",                &battle_config.item_rate_card_boss,             100,    0,      1000000,        },
	{ "item_rate_heal",                     &battle_config.item_rate_heal,                  100,    0,      1000000,        },
	{ "item_rate_heal_boss",                &battle_config.item_rate_heal_boss,             100,    0,      1000000,        },
	{ "item_rate_use",                      &battle_config.item_rate_use,                   100,    0,      1000000,        },
	{ "item_rate_use_boss",                 &battle_config.item_rate_use_boss,              100,    0,      1000000,        },
	{ "item_rate_adddrop",                  &battle_config.item_rate_adddrop,               100,    0,      1000000,        },
	{ "item_rate_treasure",                 &battle_config.item_rate_treasure,              100,    0,      1000000,        },
	{ "prevent_logout",                     &battle_config.prevent_logout,                  10000,  0,      60000,          },
	{ "alchemist_summon_reward",            &battle_config.alchemist_summon_reward,         1,      0,      2,              },
	{ "drops_by_luk",                       &battle_config.drops_by_luk,                    0,      0,      INT_MAX,        },
	{ "drops_by_luk2",                      &battle_config.drops_by_luk2,                   0,      0,      INT_MAX,        },
	{ "equip_natural_break_rate",           &battle_config.equip_natural_break_rate,        0,      0,      INT_MAX,        },
	{ "equip_self_break_rate",              &battle_config.equip_self_break_rate,           100,    0,      INT_MAX,        },
	{ "equip_skill_break_rate",             &battle_config.equip_skill_break_rate,          100,    0,      INT_MAX,        },
	{ "pk_mode",                            &battle_config.pk_mode,                         0,      0,      1,              },
	{ "pk_level_range",                     &battle_config.pk_level_range,                  0,      0,      INT_MAX,        },
	{ "manner_system",                      &battle_config.manner_system,                   0xFFF,  0,      0xFFF,          },
	{ "pet_equip_required",                 &battle_config.pet_equip_required,              0,      0,      1,              },
	{ "multi_level_up",                     &battle_config.multi_level_up,                  0,      0,      1,              },
	{ "max_exp_gain_rate",                  &battle_config.max_exp_gain_rate,               0,      0,      INT_MAX,        },
	{ "backstab_bow_penalty",               &battle_config.backstab_bow_penalty,            0,      0,      1,              },
	{ "night_at_start",                     &battle_config.night_at_start,                  0,      0,      1,              },
	{ "show_mob_info",                      &battle_config.show_mob_info,                   0,      0,      1|2|4,          },
	{ "ban_hack_trade",                     &battle_config.ban_hack_trade,                  0,      0,      INT_MAX,        },
	{ "hack_info_GM_level",                 &battle_config.hack_info_GM_level,              60,     0,      100,            },
	{ "any_warp_GM_min_level",              &battle_config.any_warp_GM_min_level,           20,     0,      100,            },
	{ "who_display_aid",                    &battle_config.who_display_aid,                 40,     0,      100,            },
	{ "packet_ver_flag",                    &battle_config.packet_ver_flag,                 0xFFFFFF,0x0000,INT_MAX,        },
	{ "min_hair_style",                     &battle_config.min_hair_style,                  0,      0,      INT_MAX,        },
	{ "max_hair_style",                     &battle_config.max_hair_style,                  23,     0,      INT_MAX,        },
	{ "min_hair_color",                     &battle_config.min_hair_color,                  0,      0,      INT_MAX,        },
	{ "max_hair_color",                     &battle_config.max_hair_color,                  9,      0,      INT_MAX,        },
	{ "min_cloth_color",                    &battle_config.min_cloth_color,                 0,      0,      INT_MAX,        },
	{ "max_cloth_color",                    &battle_config.max_cloth_color,                 4,      0,      INT_MAX,        },
	{ "pet_hair_style",                     &battle_config.pet_hair_style,                  100,    0,      INT_MAX,        },
	{ "castrate_dex_scale",                 &battle_config.castrate_dex_scale,              150,    1,      INT_MAX,        },
	{ "area_size",                          &battle_config.area_size,                       14,     0,      INT_MAX,        },
	{ "zeny_from_mobs",                     &battle_config.zeny_from_mobs,                  0,      0,      1,              },
	{ "mobs_level_up",                      &battle_config.mobs_level_up,                   0,      0,      1,              },
	{ "mobs_level_up_exp_rate",             &battle_config.mobs_level_up_exp_rate,          1,      1,      INT_MAX,        },
	{ "pk_min_level",                       &battle_config.pk_min_level,                    55,     1,      INT_MAX,        },
	{ "skill_steal_max_tries",              &battle_config.skill_steal_max_tries,           0,      0,      UCHAR_MAX,      },
	{ "motd_type",                          &battle_config.motd_type,                       0,      0,      1,              },
	{ "finding_ore_rate",                   &battle_config.finding_ore_rate,                100,    0,      INT_MAX,        },
	{ "exp_calc_type",                      &battle_config.exp_calc_type,                   0,      0,      1,              },
	{ "exp_bonus_attacker",                 &battle_config.exp_bonus_attacker,              25,     0,      INT_MAX,        },
	{ "exp_bonus_max_attacker",             &battle_config.exp_bonus_max_attacker,          12,     2,      INT_MAX,        },
	{ "min_skill_delay_limit",              &battle_config.min_skill_delay_limit,           100,    10,     INT_MAX,        },
	{ "default_walk_delay",                 &battle_config.default_walk_delay,              300,    0,      INT_MAX,        },
	{ "no_skill_delay",                     &battle_config.no_skill_delay,                  BL_MOB, BL_NUL, BL_ALL,         },
	{ "attack_walk_delay",                  &battle_config.attack_walk_delay,               BL_ALL, BL_NUL, BL_ALL,         },
	{ "require_glory_guild",                &battle_config.require_glory_guild,             0,      0,      1,              },
	{ "idle_no_share",                      &battle_config.idle_no_share,                   0,      0,      INT_MAX,        },
	{ "party_even_share_bonus",             &battle_config.party_even_share_bonus,          0,      0,      INT_MAX,        }, 
	{ "delay_battle_damage",                &battle_config.delay_battle_damage,             1,      0,      1,              },
	{ "hide_woe_damage",                    &battle_config.hide_woe_damage,                 0,      0,      1,              },
	{ "display_version",                    &battle_config.display_version,                 1,      0,      1,              },
	{ "display_hallucination",              &battle_config.display_hallucination,           1,      0,      1,              },
	{ "use_statpoint_table",                &battle_config.use_statpoint_table,             1,      0,      1,              },
	{ "ignore_items_gender",                &battle_config.ignore_items_gender,             1,      0,      1,              },
	{ "copyskill_restrict",                 &battle_config.copyskill_restrict,              2,      0,      2,              },
	{ "berserk_cancels_buffs",              &battle_config.berserk_cancels_buffs,           0,      0,      1,              },
	{ "debuff_on_logout",                   &battle_config.debuff_on_logout,                1|2,    0,      1|2,            },
	{ "monster_ai",                         &battle_config.mob_ai,                          0x000,  0x000,  0x77F,          },
	{ "hom_setting",                        &battle_config.hom_setting,                     0xFFFF, 0x0000, 0xFFFF,         },
	{ "dynamic_mobs",                       &battle_config.dynamic_mobs,                    1,      0,      1,              },
	{ "mob_remove_damaged",                 &battle_config.mob_remove_damaged,              1,      0,      1,              },
	{ "show_hp_sp_drain",                   &battle_config.show_hp_sp_drain,                0,      0,      1,              },
	{ "show_hp_sp_gain",                    &battle_config.show_hp_sp_gain,                 1,      0,      1,              },
	{ "mob_npc_event_type",                 &battle_config.mob_npc_event_type,              1,      0,      1,              },
	{ "mob_clear_delay",                    &battle_config.mob_clear_delay,                 0,      0,      INT_MAX,        },
	{ "character_size",                     &battle_config.character_size,                  1|2,    0,      1|2,            },
	{ "mob_max_skilllvl",                   &battle_config.mob_max_skilllvl,                MAX_SKILL_LEVEL, 1, MAX_SKILL_LEVEL, },
	{ "retaliate_to_master",                &battle_config.retaliate_to_master,             1,      0,      1,              },
	{ "rare_drop_announce",                 &battle_config.rare_drop_announce,              0,      0,      10000,          },
	{ "title_lvl1",                         &battle_config.title_lvl1,                      1,      0,      100,            },
	{ "title_lvl2",                         &battle_config.title_lvl2,                      10,     0,      100,            },
	{ "title_lvl3",                         &battle_config.title_lvl3,                      20,     0,      100,            },
	{ "title_lvl4",                         &battle_config.title_lvl4,                      40,     0,      100,            },
	{ "title_lvl5",                         &battle_config.title_lvl5,                      50,     0,      100,            },
	{ "title_lvl6",                         &battle_config.title_lvl6,                      60,     0,      100,            },
	{ "title_lvl7",                         &battle_config.title_lvl7,                      80,     0,      100,            },
	{ "title_lvl8",                         &battle_config.title_lvl8,                      99,     0,      100,            },
	{ "duel_allow_pvp",                     &battle_config.duel_allow_pvp,                  0,      0,      1,              },
	{ "duel_allow_gvg",                     &battle_config.duel_allow_gvg,                  0,      0,      1,              },
	{ "duel_allow_teleport",                &battle_config.duel_allow_teleport,             0,      0,      1,              },
	{ "duel_autoleave_when_die",            &battle_config.duel_autoleave_when_die,         1,      0,      1,              },
	{ "duel_time_interval",                 &battle_config.duel_time_interval,              60,     0,      INT_MAX,        },
	{ "duel_only_on_same_map",              &battle_config.duel_only_on_same_map,           0,      0,      1,              },
	{ "skip_teleport_lv1_menu",             &battle_config.skip_teleport_lv1_menu,          0,      0,      1,              },
	{ "allow_skill_without_day",            &battle_config.allow_skill_without_day,         0,      0,      1,              },
	{ "allow_es_magic_player",              &battle_config.allow_es_magic_pc,               0,      0,      1,              },
	{ "skill_caster_check",                 &battle_config.skill_caster_check,              1,      0,      1,              },
	{ "status_cast_cancel",                 &battle_config.sc_castcancel,                   BL_NUL, BL_NUL, BL_ALL,         },
	{ "pc_status_def_rate",                 &battle_config.pc_sc_def_rate,                  100,    0,      INT_MAX,        },
	{ "mob_status_def_rate",                &battle_config.mob_sc_def_rate,                 100,    0,      INT_MAX,        },
	{ "pc_luk_status_def",                  &battle_config.pc_luk_sc_def,                   300,    1,      INT_MAX,        },
	{ "mob_luk_status_def",                 &battle_config.mob_luk_sc_def,                  300,    1,      INT_MAX,        },
	{ "pc_max_status_def",                  &battle_config.pc_max_sc_def,                   100,    0,      INT_MAX,        },
	{ "mob_max_status_def",                 &battle_config.mob_max_sc_def,                  100,    0,      INT_MAX,        },
	{ "sg_miracle_skill_ratio",             &battle_config.sg_miracle_skill_ratio,          1,      0,      10000,          },
	{ "sg_angel_skill_ratio",               &battle_config.sg_angel_skill_ratio,            10,     0,      10000,          },
	{ "autospell_stacking",                 &battle_config.autospell_stacking,              0,      0,      1,              },
	{ "override_mob_names",                 &battle_config.override_mob_names,              0,      0,      2,              },
	{ "min_chat_delay",                     &battle_config.min_chat_delay,                  0,      0,      INT_MAX,        },
	{ "friend_auto_add",                    &battle_config.friend_auto_add,                 1,      0,      1,              },
	{ "hom_rename",                         &battle_config.hom_rename,                      0,      0,      1,              },
	{ "homunculus_show_growth",             &battle_config.homunculus_show_growth,          0,      0,      1,              },
	{ "homunculus_friendly_rate",           &battle_config.homunculus_friendly_rate,        100,    0,      INT_MAX,        },
	{ "vending_tax",                        &battle_config.vending_tax,                     0,      0,      10000,          },
	{ "day_duration",                       &battle_config.day_duration,                    0,      0,      INT_MAX,        },
	{ "night_duration",                     &battle_config.night_duration,                  0,      0,      INT_MAX,        },
	{ "mob_remove_delay",                   &battle_config.mob_remove_delay,                60000,  1000,   INT_MAX,        },
	{ "mob_active_time",                    &battle_config.mob_active_time,                 0,      0,      INT_MAX,        },
	{ "boss_active_time",                   &battle_config.boss_active_time,                0,      0,      INT_MAX,        },
	{ "sg_miracle_skill_duration",          &battle_config.sg_miracle_skill_duration,       3600000, 0,     INT_MAX,        },
	{ "hvan_explosion_intimate",            &battle_config.hvan_explosion_intimate,         45000,  0,      100000,         },
	{ "quest_exp_rate",                     &battle_config.quest_exp_rate,                  100,    0,      INT_MAX,        },
	{ "at_mapflag",                         &battle_config.autotrade_mapflag,               0,      0,      1,              },
	{ "at_timeout",                         &battle_config.at_timeout,                      0,      0,      INT_MAX,        },
	{ "homunculus_autoloot",                &battle_config.homunculus_autoloot,             0,      0,      1,              },
	{ "idle_no_autoloot",                   &battle_config.idle_no_autoloot,                0,      0,      INT_MAX,        },
	{ "max_guild_alliance",                 &battle_config.max_guild_alliance,              3,      0,      3,              },
	{ "ksprotection",                       &battle_config.ksprotection,                    5000,   0,      INT_MAX,        },
	{ "auction_feeperhour",                 &battle_config.auction_feeperhour,              12000,  0,      INT_MAX,        },
	{ "auction_maximumprice",               &battle_config.auction_maximumprice,            500000000, 0,   MAX_ZENY,       },
	{ "gm_viewequip_min_lv",                &battle_config.gm_viewequip_min_lv,             0,      0,      99,             },
	{ "homunculus_auto_vapor",              &battle_config.homunculus_auto_vapor,           1,      0,      1,              },
	{ "display_status_timers",              &battle_config.display_status_timers,           1,      0,      1,              },
	{ "skill_add_heal_rate",                &battle_config.skill_add_heal_rate,             7,      0,      INT_MAX,        },
	{ "eq_single_target_reflectable",       &battle_config.eq_single_target_reflectable,    1,      0,      1,              },
	{ "invincible.nodamage",                &battle_config.invincible_nodamage,             0,      0,      1,              },
	{ "mob_slave_keep_target",              &battle_config.mob_slave_keep_target,           0,      0,      1,              },
	{ "autospell_check_range",              &battle_config.autospell_check_range,           0,      0,      1,              },
	{ "client_reshuffle_dice",              &battle_config.client_reshuffle_dice,           0,      0,      1,              },
	{ "client_sort_storage",                &battle_config.client_sort_storage,             0,      0,      1,              },
	{ "gm_check_minlevel",                  &battle_config.gm_check_minlevel,               60,     0,      100,            },
	{ "feature.buying_store",               &battle_config.feature_buying_store,            1,      0,      1,              },
	{ "feature.search_stores",              &battle_config.feature_search_stores,           1,      0,      1,              },
	{ "searchstore_querydelay",             &battle_config.searchstore_querydelay,          10,     0,      INT_MAX,        },
	{ "searchstore_maxresults",             &battle_config.searchstore_maxresults,          30,     1,      INT_MAX,        },
	{ "display_party_name",                 &battle_config.display_party_name,              0,      0,      1,              },
	{ "cashshop_show_points",               &battle_config.cashshop_show_points,            0,      0,      1,              },
	{ "mail_show_status",                   &battle_config.mail_show_status,                0,      0,      2,              },
	{ "client_limit_unit_lv",               &battle_config.client_limit_unit_lv,            0,      0,      BL_ALL,         },
// BattleGround Settings
	{ "bg_update_interval",                 &battle_config.bg_update_interval,              1000,   100,    INT_MAX,        },
	{ "bg_short_attack_damage_rate",        &battle_config.bg_short_damage_rate,            80,     0,      INT_MAX,        },
	{ "bg_long_attack_damage_rate",         &battle_config.bg_long_damage_rate,             80,     0,      INT_MAX,        },
	{ "bg_weapon_attack_damage_rate",       &battle_config.bg_weapon_damage_rate,           60,     0,      INT_MAX,        },
	{ "bg_magic_attack_damage_rate",        &battle_config.bg_magic_damage_rate,            60,     0,      INT_MAX,        },
	{ "bg_misc_attack_damage_rate",         &battle_config.bg_misc_damage_rate,             60,     0,      INT_MAX,        },
	{ "bg_flee_penalty",                    &battle_config.bg_flee_penalty,                 20,     0,      INT_MAX,        },
// 3CeAM Settings
	{ "renewal_casting_renewal_skills",     &battle_config.renewal_casting_renewal_skills,  1,      0,      1,              },
	{ "castrate_dex_scale_renewal_jobs",    &battle_config.castrate_dex_scale_renewal_jobs, 150,    1,      INT_MAX,        },
	{ "max_parameter_renewal_jobs",         &battle_config.max_parameter_renewal_jobs,      120,    10,     10000,          },
	{ "max_baby_parameter_renewal_jobs",    &battle_config.max_baby_parameter_renewal_jobs, 108,    10,     10000,          },
	{ "max_aspd_renewal_jobs",              &battle_config.max_aspd_renewal_jobs,           190,    100,    199,            },
	{ "all_riding_speed",                   &battle_config.all_riding_speed,                25,     0,      100,            },
	{ "rune_produce_rate",                  &battle_config.rune_produce_rate,               100,    0,      INT_MAX,        },
	{ "player_camouflage_check_type",       &battle_config.pc_camouflage_check_type,        1,      0,      1|2|4,          },
	{ "falcon_and_wug",                     &battle_config.falcon_and_wug,                  0,      0,      1,              },
	{ "use_renewal_statpoints",             &battle_config.use_renewal_statpoints,          0,      0,      1,              },
	{ "max_highlvl_nerf",                   &battle_config.max_highlvl_nerf,                100,    0,      INT_MAX,        },
	{ "max_joblvl_nerf",                    &battle_config.max_joblvl_nerf,                 100,    0,      INT_MAX,        },
	{ "max_joblvl_nerf_misc",               &battle_config.max_joblvl_nerf_misc,            100,    0,      INT_MAX,        },
	{ "skillsbonus_maxhp_RK",               &battle_config.skillsbonus_maxhp_RK,            0,      0,      INT_MAX,        },
	{ "skillsbonus_maxhp_SR",               &battle_config.skillsbonus_maxhp_SR,            0,      0,      INT_MAX,        },
	{ "metallicsound_spburn_rate",          &battle_config.metallicsound_spburn_rate,       100,    0,      INT_MAX,        },
	{ "renewal_baselvl_skill_ratio",        &battle_config.renewal_baselvl_skill_ratio,     0,      0,      1,              },
	{ "renewal_baselvl_skill_effect",       &battle_config.renewal_baselvl_skill_effect,    1,      0,      1,              },
	{ "mado_skill_limit",                   &battle_config.mado_skill_limit,                0,      0,      1,              },
	{ "mado_loss_on_death",                 &battle_config.mado_loss_on_death,              1,      0,      1,              },
	{ "mado_cast_skill_on_limit",           &battle_config.mado_cast_skill_on_limit,        0,      0,      1,              },
	{ "marionette_renewal_jobs",            &battle_config.marionette_renewal_jobs,         0,      0,      1,              },
	{ "banana_bomb_sit_duration",           &battle_config.banana_bomb_sit_duration,        1,      0,      1,              },
	{ "monster_hp_info",                    &battle_config.monster_hp_info,                 1,      0,      1,              },
	{ "min_body_style",                     &battle_config.min_body_style,                  0,      0,      SHRT_MAX,       },
	{ "max_body_style",                     &battle_config.max_body_style,                  4,      0,      SHRT_MAX,       },
	{ "save_body_style",                    &battle_config.save_body_style,                 0,      0,      1,              },
	{ "costume_refine_def",                 &battle_config.costume_refine_def,              0,      0,      1,              },
	{ "shadow_refine_def",                  &battle_config.shadow_refine_def,               0,      0,      1,              },
	{ "cashshop_price_rate",                &battle_config.cashshop_price_rate,             100,    0,      INT_MAX,        },
	{ "hanbok_ignorepalette",               &battle_config.hanbok_ignorepalette,            0,      0,      1,              },
	{ "oktoberfest_ignorepalette",          &battle_config.oktoberfest_ignorepalette,       0,      0,      1,              },
	{ "summer2_ignorepalette",              &battle_config.summer2_ignorepalette,           0,      0,      1,              },
	{ "max_homunculus_hp",                  &battle_config.max_homunculus_hp,               32767,  100,    INT_MAX,        },
	{ "max_homunculus_sp",                  &battle_config.max_homunculus_sp,               32767,  100,    INT_MAX,        },
	{ "max_homunculus_parameter",           &battle_config.max_homunculus_parameter,        150,    10,     SHRT_MAX,       },
	{ "gc_skill_edp_boost_formula_a",       &battle_config.gc_skill_edp_boost_formula_a,    0,      0,      1000,           },
	{ "gc_skill_edp_boost_formula_b",       &battle_config.gc_skill_edp_boost_formula_b,    20,     0,      1000,           },
	{ "gc_skill_edp_boost_formula_c",       &battle_config.gc_skill_edp_boost_formula_c,    1,      0,      1,              },
};


int battle_set_value(const char* w1, const char* w2)
{
	int val = config_switch(w2);

	int i;
	ARR_FIND(0, ARRAYLENGTH(battle_data), i, strcmpi(w1, battle_data[i].str) == 0);
	if (i == ARRAYLENGTH(battle_data))
		return 0; // not found

	if (val < battle_data[i].min || val > battle_data[i].max)
	{
		ShowWarning("Value for setting '%s': %s is invalid (min:%i max:%i)! Defaulting to %i...\n", w1, w2, battle_data[i].min, battle_data[i].max, battle_data[i].defval);
		val = battle_data[i].defval;
	}

	*battle_data[i].val = val;
	return 1;
}

int battle_get_value(const char* w1)
{
	int i;
	ARR_FIND(0, ARRAYLENGTH(battle_data), i, strcmpi(w1, battle_data[i].str) == 0);
	if (i == ARRAYLENGTH(battle_data))
		return 0; // not found
	else
		return *battle_data[i].val;
}

void battle_set_defaults()
{
	int i;
	for (i = 0; i < ARRAYLENGTH(battle_data); i++)
		*battle_data[i].val = battle_data[i].defval;
}

void battle_adjust_conf()
{
	battle_config.monster_max_aspd = 2000 - battle_config.monster_max_aspd*10;
	battle_config.max_aspd = 2000 - battle_config.max_aspd*10;
	battle_config.max_aspd_renewal_jobs = 2000 - battle_config.max_aspd_renewal_jobs*10;
	battle_config.max_walk_speed = 100*DEFAULT_WALK_SPEED/battle_config.max_walk_speed;	
	battle_config.max_cart_weight *= 10;
	
	if(battle_config.max_def > 100 && !battle_config.weapon_defense_type)	 // added by [Skotlex]
		battle_config.max_def = 100;

	if(battle_config.min_hitrate > battle_config.max_hitrate)
		battle_config.min_hitrate = battle_config.max_hitrate;
		
	if(battle_config.pet_max_atk1 > battle_config.pet_max_atk2)	//Skotlex
		battle_config.pet_max_atk1 = battle_config.pet_max_atk2;
	
	if (battle_config.day_duration && battle_config.day_duration < 60000) // added by [Yor]
		battle_config.day_duration = 60000;
	if (battle_config.night_duration && battle_config.night_duration < 60000) // added by [Yor]
		battle_config.night_duration = 60000;
	
#ifndef CELL_NOSTACK
	if (battle_config.cell_stack_limit != 1)
		ShowWarning("Battle setting 'cell_stack_limit' takes no effect as this server was compiled without Cell Stack Limit support.\n");
#endif
}

int battle_config_read(const char* cfgName)
{
	char line[1024], w1[1024], w2[1024];
	FILE* fp;
	static int count = 0;

	if (count == 0)
		battle_set_defaults();

	count++;

	fp = fopen(cfgName,"r");
	if (fp == NULL)
		ShowError("File not found: %s\n", cfgName);
	else
	{
		while(fgets(line, sizeof(line), fp))
		{
			if (line[0] == '/' && line[1] == '/')
				continue;
			if (sscanf(line, "%1023[^:]:%1023s", w1, w2) != 2)
				continue;
			if (strcmpi(w1, "import") == 0)
				battle_config_read(w2);
			else
			if (battle_set_value(w1, w2) == 0)
				ShowWarning("Unknown setting '%s' in file %s\n", w1, cfgName);
		}

		fclose(fp);
	}

	count--;

	if (count == 0)
		battle_adjust_conf();

	return 0;
}

void do_init_battle(void)
{
	delay_damage_ers = ers_new(sizeof(struct delay_damage));
	add_timer_func_list(battle_delay_damage_sub, "battle_delay_damage_sub");
}

void do_final_battle(void)
{
	ers_destroy(delay_damage_ers);
}
