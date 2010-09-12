// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef _ELEMENTAL_H_
#define _ELEMENTAL_H_

#include "status.h" // struct status_data, struct status_change
#include "unit.h" // struct unit_data

#define MIN_ELETHINKTIME 100
#define MIN_ELEDISTANCE 2
#define MAX_ELEDISTANCE 6

#define EL_MODE_AGRESSIVE (MD_CANMOVE|MD_AGGRESSIVE|MD_CANATTACK)
#define EL_MODE_ASSIST (MD_CANMOVE|MD_ASSIST|MD_CANATTACK)
#define EL_MODE_PASSIVE MD_CANMOVE

#define EL_SKILLMODE_PASIVE 0x1
#define EL_SKILLMODE_ASSIST 0x2
#define EL_SKILLMODE_AGGRESSIVE 0x4

struct ele_skill {
	unsigned short id, lv;
	short mode;
};

struct s_elemental_db {
	int class_;
	char sprite[NAME_LENGTH], name[NAME_LENGTH];
	unsigned short lv;
	short range2, range3;
	struct status_data status;
	struct view_data vd;
	struct ele_skill skill[MAX_ELESKILLTREE];
};

extern struct s_elemental_db elemental_db[MAX_ELEMENTAL_CLASS];

struct elemental_data {
	struct block_list bl;
	struct unit_data ud;
	struct view_data *vd;
	struct status_data base_status, battle_status;
	struct status_change sc;
	struct regen_data regen;

	struct s_elemental_db *db;
	struct s_elemental elemental;
	char blockskill[MAX_SKILL];

	struct map_session_data *master;
	int summon_timer;
	int skill_timer;

	unsigned last_thinktime, last_linktime;
	short min_chase;
	int target_id, attacked_id;
};

bool ele_class(int class_);
struct view_data * ele_get_viewdata(int class_);

int ele_create(struct map_session_data *sd, int class_, unsigned int lifetime);
int ele_data_received(struct s_elemental *ele, bool flag);
int elemental_save(struct elemental_data *ed);

int elemental_change_mode_ack(struct elemental_data *ed, int mode);
int elemental_change_mode(struct elemental_data *ed, int mode);

void elemental_damage(struct elemental_data *ed, struct block_list *src, int hp, int sp);
void elemental_heal(struct elemental_data *ed, int hp, int sp);
int elemental_dead(struct elemental_data *ed, struct block_list *src);

int ele_delete(struct elemental_data *ed, int reply);
void ele_summon_stop(struct elemental_data *ed);

int elemental_get_lifetime(struct elemental_data *ed);

int elemental_unlocktarget(struct elemental_data *ed);
int elemental_set_target( struct map_session_data *sd, struct block_list *bl );

#define elemental_stop_walking(ed, type) unit_stop_walking(&(ed)->bl, type)
#define elemental_stop_attack(ed) unit_stop_attack(&(ed)->bl)

int do_init_elemental(void);
void read_eledb(void);
void ele_skill_reload(void);

#endif /* _ELEMENTAL_H_ */
