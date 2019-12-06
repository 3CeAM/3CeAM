// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef _ELEMENTAL_H_
#define _ELEMENTAL_H_

#include "status.h" // struct status_data, struct status_change
#include "unit.h" // struct unit_data

enum {
	ELEMTYPE_AGNI = 1,
	ELEMTYPE_AQUA,
	ELEMTYPE_VENTUS,
	ELEMTYPE_TERA,
};

enum {
	ELEMMODE_WAIT,
	ELEMMODE_PASSIVE,
	ELEMMODE_DEFENSIVE,
	ELEMMODE_OFFENSIVE,
};

struct s_elemental_db {
	int class_;
	char sprite[NAME_LENGTH], name[NAME_LENGTH];
	unsigned short lv;
	short range2, range3;
	struct status_data status;
	struct view_data vd;
	struct {
		unsigned short id, lv, mode;
	} skill[MAX_ELEMSKILL];
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
	
	unsigned water_screen_flag : 1;
};

bool elem_class(int class_);
struct view_data * elem_get_viewdata(int class_);

int elem_create(struct map_session_data *sd, int class_, unsigned int lifetime);
int elem_data_received(struct s_elemental *elem, bool flag);
int elemental_save(struct elemental_data *ed);

void elemental_damage(struct elemental_data *ed, struct block_list *src, int hp, int sp);
void elemental_heal(struct elemental_data *ed, int hp, int sp);
int elemental_dead(struct elemental_data *ed, struct block_list *src);

int elem_delete(struct elemental_data *ed, int reply);
void elem_summon_stop(struct elemental_data *ed);

int elemental_get_lifetime(struct elemental_data *ed);
int elemental_get_type(struct elemental_data *ed);

int elemental_checkskill(struct elemental_data *ed, int skill_id);

int do_init_elemental(void);

#endif /* _ELEMENTAL_H_ */
