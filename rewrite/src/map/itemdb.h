// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef _ITEMDB_H_
#define _ITEMDB_H_

#include "../common/mmo.h" // ITEM_NAME_LENGTH

#define MAX_RANDITEM	11000

// The maximum number of item delays
#define MAX_ITEMDELAYS	10

#define MAX_SEARCH	5  //Designed for search functions, species max number of matches to display.

enum item_itemid
{
	ITEMID_HOLY_WATER = 523,
	ITEMID_EMPERIUM = 714,
	ITEMID_YELLOW_GEMSTONE = 715,
	ITEMID_RED_GEMSTONE = 716,
	ITEMID_BLUE_GEMSTONE = 717,
	ITEMID_TRAP = 1065,
	ITEMID_FACE_PAINT = 6120,
	ITEMID_MAKEOVER_BRUSH = 6121,
	ITEMID_PAINT_BRUSH = 6122,
	ITEMID_SURFACE_PAINT = 6123,
	ITEMID_MAGICBOOK_FIREBOLT = 6189,
	ITEMID_MAGICBOOK_COLDBOLT,
	ITEMID_MAGICBOOK_LIGHTNINGBOLT,
	ITEMID_MAGICBOOK_STORMGUST,
	ITEMID_MAGICBOOK_VERMILION,
	ITEMID_MAGICBOOK_METEOR,
	ITEMID_MAGICBOOK_COMET,
	ITEMID_MAGICBOOK_TETRAVORTEX,
	ITEMID_MAGICBOOK_THUNDERSTORM,
	ITEMID_MAGICBOOK_JUPITEL,
	ITEMID_MAGICBOOK_WATERBALL,
	ITEMID_MAGICBOOK_HEAVENDRIVE,
	ITEMID_MAGICBOOK_EARTHSPIKE,
	ITEMID_MAGICBOOK_EARTHSTRAIN,
	ITEMID_MAGICBOOK_CHAINLIGHTNING,
	ITEMID_MAGICBOOK_CRIMSONROCK,
	ITEMID_MAGICBOOK_DRAINLIFE,
	ITEMID_SCARLETT_POINT = 6360,
	ITEMID_INDIGO_POINT,
	ITEMID_YELLOW_WISH_POINT,
	ITEMID_LIME_GREEN_POINT,
	ITEMID_STONE = 7049,
	ITEMID_SKULL_ = 7420,
	ITEMID_TOKEN_OF_SIEGFRIED = 7621,
	ITEMID_SPECIAL_ALLOY_TRAP = 7940,
	ITEMID_ANCILLA = 12333,
	ITEMID_BOARDING_HATLER = 12622,
	ITEMID_PARALYSIS_POISON = 12717,
	ITEMID_LEECH_POISON,
	ITEMID_OBLIVION_POISON,
	ITEMID_CONTAMINATION_POISON,
	ITEMID_NUMB_POISON,
	ITEMID_FEVER_POISON,
	ITEMID_LAUGHING_POISON,
	ITEMID_FATIGUE_POISON,
	ITEMID_NAUTHIZ_RUNE = 12725,
	ITEMID_RAIDO_RUNE,
	ITEMID_BERKANA_RUNE,
	ITEMID_ISA_RUNE,
	ITEMID_EIHWAZ_RUNE,
	ITEMID_URUZ_RUNE,
	ITEMID_THURISAZ_RUNE,
	ITEMID_PERTHRO_RUNE,
	ITEMID_HAGALAZ_RUNE,
	ITEMID_SNOW_FLIP = 12812,
	ITEMID_PEONY_MOMMY,
	ITEMID_SLAPPING_HERB,
	ITEMID_YGGDRASIL_DUST,
	ITEMID_APPLE_BOMB = 13260,
	ITEMID_COCONUT_BOMB,
	ITEMID_MELON_BOMB,
	ITEMID_PINEAPPLE_BOMB,
	ITEMID_BANANA_BOMB,
	ITEMID_DARK_LUMP,
	ITEMID_HARD_DARK_LUMP,
	ITEMID_VERY_HARD_DARK_LUMP,
	ITEMID_MYSTERIOUS_POWDER,
	ITEMID_THROW_BOOST500,
	ITEMID_THROW_FULL_SWING_K,
	ITEMID_THROW_MANA_PLUS,
	ITEMID_THROW_CURE_FREE,
	ITEMID_THROW_MUSTLE_M,
	ITEMID_THROW_LIFE_FORCE_F,
	ITEMID_THROW_HP_POTION_SMALL,
	ITEMID_THROW_HP_POTION_MEDIUM,
	ITEMID_THROW_HP_POTION_LARGE,
	ITEMID_THROW_SP_POTION_SMALL,
	ITEMID_THROW_SP_POTION_MEDIUM,
	ITEMID_THROW_SP_POTION_LARGE,
	ITEMID_THROW_EXTRACT_WHITE_POTION_Z,
	ITEMID_THROW_VITATA_500,
	ITEMID_THROW_EXTRACT_SALAMINE_JUICE,
	ITEMID_THROW_SAVAGE_STEAK,
	ITEMID_THROW_COCKTAIL_WARG_BLOOD,
	ITEMID_THROW_MINOR_BBQ,
	ITEMID_THROW_SIROMA_ICE_TEA,
	ITEMID_THROW_DROCERA_HERB_STEAMED,
	ITEMID_THROW_PUTTI_TAILS_NOODLES,
	ITEMID_THROW_OVERDONE_FOOD,
	ITEMID_LUX_ANIMA_RUNE = 22540,
};

#define itemid_isgemstone(id) ( (id) >= ITEMID_YELLOW_GEMSTONE && (id) <= ITEMID_BLUE_GEMSTONE )
#define itemdb_iscashfood(id) ( (id) >= 12202 && (id) <= 12207 )

//3CeAM
#define itemid_is_rune(id) ( (id) >= ITEMID_NAUTHIZ_RUNE && (id) <= ITEMID_HAGALAZ_RUNE || (id) == ITEMID_LUX_ANIMA_RUNE )
#define itemid_is_spell_book(id) ( (id) >= ITEMID_MAGICBOOK_FIREBOLT && (id) <= ITEMID_MAGICBOOK_DRAINLIFE )
#define itemid_is_guillotine_poison(id) ( (id) >= ITEMID_PARALYSIS_POISON && (id) <= ITEMID_FATIGUE_POISON )
#define itemid_is_sling_atk(id) ( (id) >= ITEMID_APPLE_BOMB && (id) <= ITEMID_VERY_HARD_DARK_LUMP )
#define itemid_is_sling_buff(id) ( (id) >= ITEMID_MYSTERIOUS_POWDER && (id) <= ITEMID_THROW_OVERDONE_FOOD )
#define itemid_is_element_point(id) ( (id) >= ITEMID_SCARLETT_POINT && (id) <= ITEMID_LIME_GREEN_POINT )
#define itemid_is_eclage_cures(id) ( (id) >= ITEMID_SNOW_FLIP && (id) <= ITEMID_YGGDRASIL_DUST )

//The only item group required by the code to be known. See const.txt for the full list.
#define IG_FINDINGORE 6
#define IG_POTION 37
//The max. item group count (increase this when needed).
#define MAX_ITEMGROUP 55

#define CARD0_FORGE 0x00FF
#define CARD0_CREATE 0x00FE
#define CARD0_PET ((short)0xFF00)

//Marks if the card0 given is "special" (non-item id used to mark pets/created items. [Skotlex]
#define itemdb_isspecial(i) (i == CARD0_FORGE || i == CARD0_CREATE || i == CARD0_PET)

//Use apple for unknown items.
#define UNKNOWN_ITEM_ID 512

struct item_data {
	int nameid;
	char name[ITEM_NAME_LENGTH],jname[ITEM_NAME_LENGTH];
	//Do not add stuff between value_buy and wlv (see how getiteminfo works)
	int value_buy;
	int value_sell;
	int type;
	int maxchance; //For logs, for external game info, for scripts: Max drop chance of this item (e.g. 0.01% , etc.. if it = 0, then monsters don't drop it, -1 denotes items sold in shops only) [Lupus]
	int sex;
	int equip;
	int weight;
	int atk;
	int def;
	int range;
	int slot;
	int look;
	int elv;
	int wlv;
	int view_id;
	int delay;
//Lupus: I rearranged order of these fields due to compatibility with ITEMINFO script command
//		some script commands should be revised as well...
	unsigned int class_base[3];	//Specifies if the base can wear this item (split in 3 indexes per type: 1-1, 2-1, 2-2)
	unsigned class_upper : 4; //Specifies if the upper-type can equip it (bitfield, 1: normal, 2: upper, 3: baby, 4:third)
	struct {
		unsigned short chance;
		int id;
	} mob[MAX_SEARCH]; //Holds the mobs that have the highest drop rate for this item. [Skotlex]
	struct script_code *script;	//Default script for everything.
	struct script_code *equip_script;	//Script executed once when equipping.
	struct script_code *unequip_script;//Script executed once when unequipping.
	struct {
		unsigned available : 1;
		short no_equip;
		unsigned no_refine : 1;	// [celest]
		unsigned delay_consume : 1;	// Signifies items that are not consumed immediately upon double-click [Skotlex]
		unsigned trade_restriction : 7;	//Item restrictions mask [Skotlex]
		unsigned autoequip: 1;
		unsigned buyingstore : 1;
	} flag;
	short gm_lv_trade_override;	//GM-level to override trade_restriction
};

struct item_group {
	int nameid[MAX_RANDITEM];
	int qty; //Counts amount of items in the group.
};

struct item_data* itemdb_searchname(const char *name);
int itemdb_searchname_array(struct item_data** data, int size, const char *str);
struct item_data* itemdb_load(int nameid);
struct item_data* itemdb_search(int nameid);
struct item_data* itemdb_exists(int nameid);
#define itemdb_name(n) itemdb_search(n)->name
#define itemdb_jname(n) itemdb_search(n)->jname
#define itemdb_type(n) itemdb_search(n)->type
#define itemdb_atk(n) itemdb_search(n)->atk
#define itemdb_def(n) itemdb_search(n)->def
#define itemdb_look(n) itemdb_search(n)->look
#define itemdb_weight(n) itemdb_search(n)->weight
#define itemdb_equip(n) itemdb_search(n)->equip
#define itemdb_usescript(n) itemdb_search(n)->script
#define itemdb_equipscript(n) itemdb_search(n)->script
#define itemdb_wlv(n) itemdb_search(n)->wlv
#define itemdb_range(n) itemdb_search(n)->range
#define itemdb_slot(n) itemdb_search(n)->slot
#define itemdb_available(n) (itemdb_search(n)->flag.available)
#define itemdb_viewid(n) (itemdb_search(n)->view_id)
#define itemdb_autoequip(n) (itemdb_search(n)->flag.autoequip)
const char* itemdb_typename(int type);

int itemdb_group_bonus(struct map_session_data* sd, int itemid);
int itemdb_searchrandomid(int flags);

#define itemdb_value_buy(n) itemdb_search(n)->value_buy
#define itemdb_value_sell(n) itemdb_search(n)->value_sell
#define itemdb_canrefine(n) itemdb_search(n)->flag.no_refine
//Item trade restrictions [Skotlex]
int itemdb_isdropable_sub(struct item_data *, int, int);
int itemdb_cantrade_sub(struct item_data*, int, int);
int itemdb_canpartnertrade_sub(struct item_data*, int, int);
int itemdb_cansell_sub(struct item_data*,int, int);
int itemdb_cancartstore_sub(struct item_data*, int, int);
int itemdb_canstore_sub(struct item_data*, int, int);
int itemdb_canguildstore_sub(struct item_data*, int, int);
int itemdb_isrestricted(struct item* item, int gmlv, int gmlv2, int (*func)(struct item_data*, int, int));
#define itemdb_isdropable(item, gmlv) itemdb_isrestricted(item, gmlv, 0, itemdb_isdropable_sub)
#define itemdb_cantrade(item, gmlv, gmlv2) itemdb_isrestricted(item, gmlv, gmlv2, itemdb_cantrade_sub)
#define itemdb_canpartnertrade(item, gmlv, gmlv2) itemdb_isrestricted(item, gmlv, gmlv2, itemdb_canpartnertrade_sub)
#define itemdb_cansell(item, gmlv) itemdb_isrestricted(item, gmlv, 0, itemdb_cansell_sub)
#define itemdb_cancartstore(item, gmlv)  itemdb_isrestricted(item, gmlv, 0, itemdb_cancartstore_sub)
#define itemdb_canstore(item, gmlv) itemdb_isrestricted(item, gmlv, 0, itemdb_canstore_sub) 
#define itemdb_canguildstore(item, gmlv) itemdb_isrestricted(item , gmlv, 0, itemdb_canguildstore_sub) 

int itemdb_isequip(int);
int itemdb_isequip2(struct item_data *);
int itemdb_isidentified(int);
int itemdb_isstackable(int);
int itemdb_isstackable2(struct item_data *);

void itemdb_reload(void);

void do_final_itemdb(void);
int do_init_itemdb(void);

#endif /* _ITEMDB_H_ */
