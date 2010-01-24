// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef _STATUS_H_
#define _STATUS_H_

struct block_list;
struct mob_data;
struct pet_data;
struct homun_data;
struct status_change;

//Use this to refer the max refinery level [Skotlex]
#define MAX_REFINE 10
#define MAX_REFINE_BONUS 5

extern unsigned long StatusChangeFlagTable[];


// Status changes listing. These code are for use by the server. 
typedef enum sc_type {
	SC_NONE = -1,

	//First we enumerate common status ailments which are often used around.
	SC_STONE = 0,
	SC_COMMON_MIN = 0, // begin
	SC_FREEZE,
	SC_STUN,
	SC_SLEEP,
	SC_POISON,
	SC_CURSE,
	SC_SILENCE,
	SC_CONFUSION,
	SC_BLIND,
	SC_BLEEDING,
	SC_DPOISON, //10
	SC_COMMON_MAX = 10, // end
	
	//Next up, we continue on 20, to leave enough room for additional "common" ailments in the future.
	SC_PROVOKE = 20,
	SC_ENDURE,
	SC_TWOHANDQUICKEN,
	SC_CONCENTRATE,
	SC_HIDING,
	SC_CLOAKING,
	SC_ENCPOISON,
	SC_POISONREACT,
	SC_QUAGMIRE,
	SC_ANGELUS,
	SC_BLESSING, //30
	SC_SIGNUMCRUCIS,
	SC_INCREASEAGI,
	SC_DECREASEAGI,
	SC_SLOWPOISON,
	SC_IMPOSITIO  ,
	SC_SUFFRAGIUM,
	SC_ASPERSIO,
	SC_BENEDICTIO,
	SC_KYRIE,
	SC_MAGNIFICAT, //40
	SC_GLORIA,
	SC_AETERNA,
	SC_ADRENALINE,
	SC_WEAPONPERFECTION,
	SC_OVERTHRUST,
	SC_MAXIMIZEPOWER,
	SC_TRICKDEAD,
	SC_LOUD,
	SC_ENERGYCOAT,
	SC_BROKENARMOR, //50 - NOTE: These two aren't used anywhere, and they have an icon...
	SC_BROKENWEAPON,
	SC_HALLUCINATION,
	SC_WEIGHT50,
	SC_WEIGHT90,
	SC_ASPDPOTION0,
	SC_ASPDPOTION1,
	SC_ASPDPOTION2,
	SC_ASPDPOTION3,
	SC_SPEEDUP0,
	SC_SPEEDUP1, //60
	SC_ATKPOTION,
	SC_MATKPOTION,
	SC_WEDDING,
	SC_SLOWDOWN,
	SC_ANKLE,
	SC_KEEPING,
	SC_BARRIER,
	SC_STRIPWEAPON,
	SC_STRIPSHIELD,
	SC_STRIPARMOR, //70
	SC_STRIPHELM,
	SC_CP_WEAPON,
	SC_CP_SHIELD,
	SC_CP_ARMOR,
	SC_CP_HELM,
	SC_AUTOGUARD,
	SC_REFLECTSHIELD,
	SC_SPLASHER,
	SC_PROVIDENCE,
	SC_DEFENDER, //80
	SC_MAGICROD,
	SC_SPELLBREAKER,
	SC_AUTOSPELL,
	SC_SIGHTTRASHER,
	SC_AUTOBERSERK,
	SC_SPEARQUICKEN,
	SC_AUTOCOUNTER,
	SC_SIGHT,
	SC_SAFETYWALL,
	SC_RUWACH, //90
	SC_EXTREMITYFIST,
	SC_EXPLOSIONSPIRITS,
	SC_COMBO,
	SC_BLADESTOP_WAIT,
	SC_BLADESTOP,
	SC_FIREWEAPON,
	SC_WATERWEAPON,
	SC_WINDWEAPON,
	SC_EARTHWEAPON,
	SC_VOLCANO, //100,
	SC_DELUGE,
	SC_VIOLENTGALE,
	SC_WATK_ELEMENT,
	SC_ARMOR,
	SC_ARMOR_ELEMENT,
	SC_NOCHAT,
	SC_BABY,
	SC_AURABLADE,
	SC_PARRYING,
	SC_CONCENTRATION, //110
	SC_TENSIONRELAX,
	SC_BERSERK,
	SC_FURY,
	SC_GOSPEL,
	SC_ASSUMPTIO,
	SC_BASILICA,
	SC_GUILDAURA,
	SC_MAGICPOWER,
	SC_EDP,
	SC_TRUESIGHT, //120
	SC_WINDWALK,
	SC_MELTDOWN,
	SC_CARTBOOST,
	SC_CHASEWALK,
	SC_REJECTSWORD,
	SC_MARIONETTE,
	SC_MARIONETTE2,
	SC_CHANGEUNDEAD,
	SC_JOINTBEAT,
	SC_MINDBREAKER, //130
	SC_MEMORIZE,
	SC_FOGWALL,
	SC_SPIDERWEB,
	SC_DEVOTION,
	SC_SACRIFICE,
	SC_STEELBODY,
	SC_ORCISH,
	SC_READYSTORM,
	SC_READYDOWN,
	SC_READYTURN, //140
	SC_READYCOUNTER,
	SC_DODGE,
	SC_RUN,
	SC_SHADOWWEAPON,
	SC_ADRENALINE2,
	SC_GHOSTWEAPON,
	SC_KAIZEL,
	SC_KAAHI,
	SC_KAUPE,
	SC_ONEHAND, //150
	SC_PRESERVE,
	SC_BATTLEORDERS,
	SC_REGENERATION,
	SC_DOUBLECAST,
	SC_GRAVITATION,
	SC_MAXOVERTHRUST,
	SC_LONGING,
	SC_HERMODE,
	SC_SHRINK,
	SC_SIGHTBLASTER, //160
	SC_WINKCHARM,
	SC_CLOSECONFINE,
	SC_CLOSECONFINE2,
	SC_DANCING,
	SC_ELEMENTALCHANGE,
	SC_RICHMANKIM,
	SC_ETERNALCHAOS,
	SC_DRUMBATTLE,
	SC_NIBELUNGEN,
	SC_ROKISWEIL, //170
	SC_INTOABYSS,
	SC_SIEGFRIED,
	SC_WHISTLE,
	SC_ASSNCROS,
	SC_POEMBRAGI,
	SC_APPLEIDUN,
	SC_MODECHANGE,
	SC_HUMMING,
	SC_DONTFORGETME,
	SC_FORTUNE, //180
	SC_SERVICE4U,
	SC_STOP,	//Prevents inflicted chars from walking. [Skotlex]
	SC_SPURT,
	SC_SPIRIT,
	SC_COMA, //Not a real SC_, it makes a char's HP/SP hit 1.
	SC_INTRAVISION,
	SC_INCALLSTATUS,
	SC_INCSTR,
	SC_INCAGI,
	SC_INCVIT, //190
	SC_INCINT,
	SC_INCDEX,
	SC_INCLUK,
	SC_INCHIT,
	SC_INCHITRATE,
	SC_INCFLEE,
	SC_INCFLEERATE,
	SC_INCMHPRATE,
	SC_INCMSPRATE,
	SC_INCATKRATE, //200
	SC_INCMATKRATE,
	SC_INCDEFRATE,
	SC_STRFOOD,
	SC_AGIFOOD,
	SC_VITFOOD,
	SC_INTFOOD,
	SC_DEXFOOD,
	SC_LUKFOOD,
	SC_HITFOOD,
	SC_FLEEFOOD, //210
	SC_BATKFOOD,
	SC_WATKFOOD,
	SC_MATKFOOD,
	SC_SCRESIST, //Increases resistance to status changes.
	SC_XMAS, // Xmas Suit [Valaris]
	SC_WARM, //SG skills [Komurka]
	SC_SUN_COMFORT,
	SC_MOON_COMFORT,
	SC_STAR_COMFORT,
	SC_FUSION, //220
	SC_SKILLRATE_UP,
	SC_SKE,
	SC_KAITE,
	SC_SWOO, // [marquis007]
	SC_SKA, // [marquis007]
	SC_EARTHSCROLL,
	SC_MIRACLE, //SG 'hidden' skill [Komurka]
	SC_MADNESSCANCEL,
	SC_ADJUSTMENT,
	SC_INCREASING,  //230
	SC_GATLINGFEVER,
	SC_TATAMIGAESHI,
	SC_UTSUSEMI,
	SC_BUNSINJYUTSU,
	SC_KAENSIN,
	SC_SUITON,
	SC_NEN,
	SC_KNOWLEDGE,
	SC_SMA,
	SC_FLING,	//240
	SC_AVOID,
	SC_CHANGE,
	SC_BLOODLUST,
	SC_FLEET,
	SC_SPEED,
	SC_DEFENCE,
	//SC_INCASPDRATE,
	SC_INCFLEE2 = 248,
	SC_JAILED,
	SC_ENCHANTARMS,	//250
	SC_MAGICALATTACK,
	SC_ARMORCHANGE,
	SC_CRITICALWOUND,
	SC_MAGICMIRROR,
	SC_SLOWCAST,
	SC_SUMMER,
	SC_EXPBOOST,
	SC_ITEMBOOST,
	SC_BOSSMAPINFO, 
	SC_LIFEINSURANCE, //260
	SC_INCCRI,
	//SC_INCDEF,
	//SC_INCBASEATK = 263,
	//SC_FASTCAST,
	SC_MDEF_RATE = 265,
	//SC_HPREGEN,
	SC_INCHEALRATE = 267,
	SC_PNEUMA,
	SC_AUTOTRADE,
	SC_KSPROTECTED, //270
	SC_ARMOR_RESIST = 271,
	SC_SPCOST_RATE,
	SC_COMMONSC_RESIST,
	SC_SEVENWIND,
	SC_DEF_RATE,
	//SC_SPREGEN,
	SC_WALKSPEED = 277,

	// Mercenary Only Bonus Effects
	SC_MERC_FLEEUP,
	SC_MERC_ATKUP,
	SC_MERC_HPUP, //280
	SC_MERC_SPUP,
	SC_MERC_HITUP,
	SC_MERC_QUICKEN,

	SC_REBIRTH,
	//SC_SKILLCASTRATE, //285
	//SC_DEFRATIOATK,
	//SC_HPDRAIN,
	//SC_SKILLATKBONUS,
	SC_ITEMSCRIPT = 289,
	SC_S_LIFEPOTION, //290
	SC_L_LIFEPOTION,
	SC_JEXPBOOST,
	//SC_IGNOREDEF,
	SC_HELLPOWER = 294,
	SC_INVINCIBLE, //295
	SC_INVINCIBLEOFF,


	SC_FREEZING = 330,
	SC_BURNING,
	SC_FEAR,
	SC_ENCHANTBLADE = 350,
	SC_DEATHBOUND,
	SC_NAUTHIZ,
	SC_RAIDO,
	SC_BERKANA,
	SC_ISA,
	SC_OTHILA,
	SC_URUZ,
	SC_THURISAZ,
	SC_WYRD,
	SC_HAGALAZ,
	SC_EPICLESIS,
	SC_ORATIO,
	SC_LAUDAAGNUS,
	SC_LAUDARAMUS,
	SC_RENOVATIO,
	SC_EXPIATIO,
	SC_DUPLELIGHT,
	SC_SECRAMENT,

	SC_WHITEIMPRISON,
	SC_RECOGNIZEDSPELL,
	SC_MARSHOFABYSS,
	SC_STASIS,
	SC_CHAINLIGHTNING,
	SC_SPHERE_1,
	SC_SPHERE_2,
	SC_SPHERE_3,
	SC_SPHERE_4,
	SC_SPHERE_5,
	SC_FREEZINGSPELL,

	SC_FEARBREEZE,
	SC_ELECTRICSHOCKER,
	SC_BITE,
	SC_WUGDASH,
	SC_CAMOUFLAGE,

	SC_ACCELERATION,
	SC_INFRAREDSCAN,

	SC__ENERVATION,
	SC__GROOMY,
	SC__IGNORANCE,
	SC__LAZINESS,
	SC__UNLUCKY,
	SC__WEAKNESS,
	SC__REPRODUCE,
	SC__AUTOSHADOWSPELL,
	SC__SHADOWFORM,
	SC__STRIPACCESSORY,
	SC__BODYPAINT,
	SC__INVISIBILITY,
	SC__DEADLYINFECT,
	SC__MANHOLE,
	SC_CHAOS,
	SC__BLOODYLUST,

	SC_GN_CARTBOOST,
	SC_BLOODSUCKER,
	SC_THORNSTRAP,
	SC_SMOKEPOWDER,
	SC_TEARGAS,
	SC_MANDRAGORA,

	SC_FIREWALK,
	SC_ELECTRICWALK,
	SC_DIAMONDDUST,
	SC_WARMER,
	SC_STRIKING,
	SC_DEEPSLEEP,

	SC_MAX, //Automatically updated max, used in for's to check we are within bounds.
} sc_type;

// Official status change ids, used to display status icons on the client.
enum si_type {
	SI_BLANK		= -1,
	SI_PROVOKE		= 0,
	SI_ENDURE		= 1,
	SI_TWOHANDQUICKEN	= 2,
	SI_CONCENTRATE		= 3,
	SI_HIDING		= 4,
	SI_CLOAKING		= 5,
	SI_ENCPOISON		= 6,
	SI_POISONREACT		= 7,
	SI_QUAGMIRE		= 8,
	SI_ANGELUS		= 9,
	SI_BLESSING		= 10,
	SI_SIGNUMCRUCIS		= 11,
	SI_INCREASEAGI		= 12,
	SI_DECREASEAGI		= 13,
	SI_SLOWPOISON		= 14,
	SI_IMPOSITIO  		= 15,
	SI_SUFFRAGIUM		= 16,
	SI_ASPERSIO		= 17,
	SI_BENEDICTIO		= 18,
	SI_KYRIE		= 19,
	SI_MAGNIFICAT		= 20,
	SI_GLORIA		= 21,
	SI_AETERNA		= 22,
	SI_ADRENALINE		= 23,
	SI_WEAPONPERFECTION	= 24,
	SI_OVERTHRUST		= 25,
	SI_MAXIMIZEPOWER	= 26,
	SI_RIDING		= 27,
	SI_FALCON		= 28,
	SI_TRICKDEAD		= 29,
	SI_LOUD			= 30,
	SI_ENERGYCOAT		= 31,
	SI_BROKENARMOR		= 32,
	SI_BROKENWEAPON		= 33,
	SI_HALLUCINATION	= 34,
	SI_WEIGHT50 		= 35,
	SI_WEIGHT90		= 36,
	SI_ASPDPOTION0		= 37,
	SI_ASPDPOTION1 = 38,
	SI_ASPDPOTION2 = 39,
	SI_ASPDPOTIONINFINITY = 40,
	SI_SPEEDPOTION1		= 41,
	SI_SPEEDPOTION2		= 42,
//	SI_AUTOCOUNTER = 43,
//	SI_SPLASHER = 44,
//	SI_ANKLESNARE = 45,
	SI_ACTIONDELAY		= 46,
//	SI_NOACTION = 47,
//	SI_IMPOSSIBLEPICKUP = 48,
//	SI_BARRIER = 49,
	SI_STRIPWEAPON		= 50,
	SI_STRIPSHIELD		= 51,
	SI_STRIPARMOR		= 52,
	SI_STRIPHELM		= 53,
	SI_CP_WEAPON		= 54,
	SI_CP_SHIELD		= 55,
	SI_CP_ARMOR		= 56,
	SI_CP_HELM		= 57,
	SI_AUTOGUARD		= 58,
	SI_REFLECTSHIELD	= 59,
//	SI_DEVOTION = 60,
	SI_PROVIDENCE		= 61,
	SI_DEFENDER		= 62,
//	SI_MAGICROD = 63,
//	SI_WEAPONPROPERTY = 64,
	SI_AUTOSPELL		= 65,
//	SI_SPECIALZONE = 66,
//	SI_MASK = 67,
	SI_SPEARQUICKEN		= 68,
//	SI_BDPLAYING = 69,
//	SI_WHISTLE = 70,
//	SI_ASSASSINCROSS = 71,
//	SI_POEMBRAGI = 72,
//	SI_APPLEIDUN = 73,
//	SI_HUMMING = 74,
//	SI_DONTFORGETME = 75,
//	SI_FORTUNEKISS = 76,
//	SI_SERVICEFORYOU = 77,
//	SI_RICHMANKIM = 78,
//	SI_ETERNALCHAOS = 79,
//	SI_DRUMBATTLEFIELD = 80,
//	SI_RINGNIBELUNGEN = 81,
//	SI_ROKISWEIL = 82,
//	SI_INTOABYSS = 83,
//	SI_SIEGFRIED = 84,
//	SI_BLADESTOP = 85,
	SI_EXPLOSIONSPIRITS	= 86,
	SI_STEELBODY		= 87,
//	SI_EXTREMITYFIST = 88,
//	SI_COMBOATTACK = 89,
	SI_FIREWEAPON		= 90,
	SI_WATERWEAPON		= 91,
	SI_WINDWEAPON		= 92,
	SI_EARTHWEAPON		= 93,
//	SI_MAGICATTACK = 94,
	SI_STOP			= 95,
//	SI_WEAPONBRAKER = 96,
	SI_UNDEAD		= 97,
//	SI_POWERUP = 98,
//	SI_AGIUP = 99,
//	SI_SIEGEMODE = 100,
//	SI_INVISIBLE = 101,
//	SI_STATUSONE = 102,
	SI_AURABLADE		= 103,
	SI_PARRYING		= 104,
	SI_CONCENTRATION	= 105,
	SI_TENSIONRELAX		= 106,
	SI_BERSERK		= 107,
//	SI_SACRIFICE = 108,
//	SI_GOSPEL = 109,
	SI_ASSUMPTIO		= 110,
//	SI_BASILICA = 111,
	SI_LANDENDOW		= 112,
	SI_MAGICPOWER		= 113,
	SI_EDP			= 114,
	SI_TRUESIGHT		= 115,
	SI_WINDWALK		= 116,
	SI_MELTDOWN		= 117,
	SI_CARTBOOST		= 118,
//	SI_CHASEWALK = 119,
	SI_REJECTSWORD		= 120,
	SI_MARIONETTE		= 121,
	SI_MARIONETTE2		= 122,
	SI_MOONLIT		= 123,
	SI_BLEEDING		= 124,
	SI_JOINTBEAT		= 125,
//	SI_MINDBREAKER = 126,
//	SI_MEMORIZE = 127,
//	SI_FOGWALL = 128,
//	SI_SPIDERWEB = 129,
	SI_BABY			= 130,
//	SI_SUB_WEAPONPROPERTY = 131,
	SI_AUTOBERSERK		= 132,
	SI_RUN			= 133,
	SI_BUMP			= 134,
	SI_READYSTORM		= 135,
//	SI_STORMKICK_READY = 136,
	SI_READYDOWN		= 137,
//	SI_DOWNKICK_READY = 138,
	SI_READYTURN		= 139,
//	SI_TURNKICK_READY = 140,
	SI_READYCOUNTER		= 141,
//	SI_COUNTER_READY = 142,
	SI_DODGE		= 143,
//	SI_DODGE_READY = 144,
	SI_SPURT		= 145,
	SI_SHADOWWEAPON		= 146,
	SI_ADRENALINE2		= 147,
	SI_GHOSTWEAPON		= 148,
	SI_SPIRIT		= 149,
//	SI_PLUSATTACKPOWER = 150,
//	SI_PLUSMAGICPOWER = 151,
	SI_DEVIL		= 152,
	SI_KAITE		= 153,
//	SI_SWOO = 154,
//	SI_STAR2 = 155,
	SI_KAIZEL		= 156,
	SI_KAAHI		= 157,
	SI_KAUPE		= 158,
	SI_SMA			= 159,
	SI_NIGHT		= 160,
	SI_ONEHAND		= 161,
//	SI_FRIEND = 162,
//	SI_FRIENDUP = 163,
//	SI_SG_WARM = 164,
	SI_WARM			= 165,	
//	166 | The three show the exact same display: ultra red character (165, 166, 167)	
//	167 | Their names would be SI_SG_SUN_WARM, SI_SG_MOON_WARM, SI_SG_STAR_WARM
//	SI_EMOTION = 168,
	SI_SUN_COMFORT		= 169,
	SI_MOON_COMFORT		= 170,	
	SI_STAR_COMFORT		= 171,	
//	SI_EXPUP = 172,
//	SI_GDSKILL_BATTLEORDER = 173,
//	SI_GDSKILL_REGENERATION = 174,
//	SI_GDSKILL_POSTDELAY = 175,
//	SI_RESISTHANDICAP = 176,
//	SI_MAXHPPERCENT = 177,
//	SI_MAXSPPERCENT = 178,
//	SI_DEFENCE = 179,
//	SI_SLOWDOWN = 180,
	SI_PRESERVE		= 181,
	SI_INCSTR		= 182,
//	SI_NOT_EXTREMITYFIST = 183,
	SI_INTRAVISION		= 184,
//	SI_MOVESLOW_POTION = 185,
	SI_DOUBLECAST		= 186,
//	SI_GRAVITATION = 187,
	SI_MAXOVERTHRUST	= 188,
//	SI_LONGING = 189,
//	SI_HERMODE = 190,
	SI_TAROT		= 191, // the icon allows no doubt... but what is it really used for ?? [DracoRPG]
//	SI_HLIF_AVOID = 192,
//	SI_HFLI_FLEET = 193,
//	SI_HFLI_SPEED = 194,
//	SI_HLIF_CHANGE = 195,
//	SI_HAMI_BLOODLUST = 196,
	SI_SHRINK		= 197,
	SI_SIGHTBLASTER		= 198,
	SI_WINKCHARM		= 199,
	SI_CLOSECONFINE		= 200,
	SI_CLOSECONFINE2	= 201,
//	SI_DISABLEMOVE = 202,
	SI_MADNESSCANCEL	= 203,	//[blackhole89]
	SI_GATLINGFEVER		= 204,
//	SI_EARTHSCROLL = 205,
	SI_UTSUSEMI		= 206,
	SI_BUNSINJYUTSU		= 207,
	SI_NEN			= 208,
	SI_ADJUSTMENT		= 209,
	SI_ACCURACY		= 210,
//	SI_NJ_SUITON = 211,
//	SI_PET = 212,
//	SI_MENTAL = 213,
//	SI_EXPMEMORY = 214,
//	SI_PERFORMANCE = 215,
//	SI_GAIN = 216,
//	SI_GRIFFON = 217,
//	SI_DRIFT = 218,
//	SI_WALLSHIFT = 219,
//	SI_REINCARNATION = 220,
//	SI_PATTACK = 221,
//	SI_PSPEED = 222,
//	SI_PDEFENSE = 223,
//	SI_PCRITICAL = 224,
//	SI_RANKING = 225,
//	SI_PTRIPLE = 226,
//	SI_DENERGY = 227,
//	SI_WAVE1 = 228,
//	SI_WAVE2 = 229,
//	SI_WAVE3 = 230,
//	SI_WAVE4 = 231,
//	SI_DAURA = 232,
//	SI_DFREEZER = 233,
//	SI_DPUNISH = 234,
//	SI_DBARRIER = 235,
//	SI_DWARNING = 236,
//	SI_MOUSEWHEEL = 237,
//	SI_DGAUGE = 238,
//	SI_DACCEL = 239,
//	SI_DBLOCK = 240,
	SI_FOODSTR		= 241,
	SI_FOODAGI		= 242,
	SI_FOODVIT		= 243,
	SI_FOODDEX		= 244,
	SI_FOODINT		= 245,
	SI_FOODLUK		= 246,
	SI_FOODFLEE		= 247,
	SI_FOODHIT		= 248,
	SI_FOODCRI		= 249,
	SI_EXPBOOST		= 250,
	SI_LIFEINSURANCE	= 251,
	SI_ITEMBOOST		= 252,
	SI_BOSSMAPINFO		= 253,
//	SI_DA_ENERGY = 254,
//	SI_DA_FIRSTSLOT = 255,
//	SI_DA_HEADDEF = 256,
//	SI_DA_SPACE = 257,
//	SI_DA_TRANSFORM = 258,
//	SI_DA_ITEMREBUILD = 259,
//	SI_DA_ILLUSION = 260, //All mobs display as Turtle General
//	SI_DA_DARKPOWER = 261,
//	SI_DA_EARPLUG = 262,
//	SI_DA_CONTRACT = 263, //Bio Mob effect on you and SI_TRICKDEAD icon
//	SI_DA_BLACK = 264, //For short time blurry screen
//	SI_DA_MAGICCART = 265,
//	SI_CRYSTAL = 266,
//	SI_DA_REBUILD = 267,
//	SI_DA_EDARKNESS = 268,
//	SI_DA_EGUARDIAN = 269,
//	SI_DA_TIMEOUT = 270,
//	SI_FOOD_STR_CASH = 271,
//	SI_FOOD_AGI_CASH = 272,
//	SI_FOOD_VIT_CASH = 273,
//	SI_FOOD_DEX_CASH = 274,
//	SI_FOOD_INT_CASH = 275,
//	SI_FOOD_LUK_CASH = 276,
	SI_MERC_FLEEUP	= 277,
	SI_MERC_ATKUP	= 278,
	SI_MERC_HPUP	= 279,
	SI_MERC_SPUP	= 280,
	SI_MERC_HITUP	= 281,
	SI_SLOWCAST		= 282,
//	SI_MAGICMIRROR = 283,
//	SI_STONESKIN = 284,
//	SI_ANTIMAGIC = 285,
	SI_CRITICALWOUND	= 286,
//	SI_NPC_DEFENDER = 287,
//	SI_NOACTION_WAIT = 288,
//	SI_MOVHASTE_HORSE = 289,
	SI_DEF_RATE		= 290,
	SI_MDEF_RATE	= 291,
	SI_INCHEALRATE	= 292,
	SI_S_LIFEPOTION = 293,
	SI_L_LIFEPOTION = 294,
	SI_INCCRI		= 295,
	SI_PLUSAVOIDVALUE = 296,
//	SI_ATKER_ASPD = 297,
//	SI_TARGET_ASPD = 298,
//	SI_ATKER_MOVESPEED = 299,
	SI_ATKER_BLOOD = 300,
	SI_TARGET_BLOOD = 301,
	SI_ARMOR_PROPERTY = 302,
//	SI_REUSE_LIMIT_A = 303,
	SI_HELLPOWER = 304,
//	SI_STEAMPACK = 305,
//	SI_REUSE_LIMIT_B = 306,
//	SI_REUSE_LIMIT_C = 307,
//	SI_REUSE_LIMIT_D = 308,
//	SI_REUSE_LIMIT_E = 309,
//	SI_REUSE_LIMIT_F = 310,
	SI_INVINCIBLE = 311,
	SI_CASH_PLUSONLYJOBEXP = 312,
//	SI_PARTYFLEE = 313,
//	SI_ANGEL_PROTECT = 314,
// 315
	SI_ENCHANTBLADE = 316,
	SI_DEATHBOUND = 317,
	SI_NAUTHIZ = 318,
	SI_TURISUSS = 319,
	SI_HAGALAZ = 320,
	SI_ISHA = 321,
	SI_EISIR = 322,
	SI_URUZ = 323,
	SI_BERKANA = 324,
	SI_RAIDO = 325,
	SI_WYRD = 326,
// 327
	SI_VENOMIMPRESS = 328,
// 329
	SI_ORATIO = 330,
// 331-332
	SI_POISONINGWEAPON = 333, //used to be the SI of Cloaking Exceed and Poison Smoke.
	SI_HALLUCINATIONWALK = 334,
// 335
	SI_RENOVATIO = 336,
	SI_WEAPONBLOCKING = 337,
// 338
	SI_ROLLINGCUTTER = 339,
// 340
	SI_PARALYSIS = 341, //used to be the SI of Poisoning Weapon and Paralysis
	SI_TOXIN = 342,
//	SI_PARALYSIS_2 = 343,
	SI_FATIGUE = 344,
	SI_LAUGHING = 345,
	SI_DISHEART = 346,
	SI_PYREXIA = 347,
	SI_OBLIVION = 348,
	SI_LEECH = 349,
	SI_DUPLELIGHT = 350,
	SI_FREEZING = 351,
	SI_FEARBREEZE = 352,
// 353
	SI_MARSHOFABYSS = 354,
	SI_RECOGNIZEDSPELL = 355,
// 356
	SI_WOLFMOUNT = 357,
	SI_WOLFDASH = 358,
// 359
	SI_CAMOUFLAGE = 360,
	SI_ACCELERATION = 361,
	SI_HOVERING = 362,
	SI_SPHERE_1 = 363,
	SI_SPHERE_2 = 364,
	SI_SPHERE_3 = 365,
	SI_SPHERE_4 = 366,
	SI_SPHERE_5 = 367,
// 368-372
	SI_OVERHEAT = 373,
	SI_SHAPESHIFT = 374,
	SI_INFRAREDSCAN = 375,
// 376
	SI_NEUTRALBARRIER = 377,
// 378
	SI_STEALTHFIELD = 379,
// 380
//	SI_EFST_ATK_1 = 381,
//	SI_EFST_DEF_1 = 382,
//	SI_EFST_ATK_2 = 383,
//	SI_EFST_DEF_2 = 384,
	SI_REPRODUCE = 385,
//	SI_EFST_MATK_1 = 386,
//	SI_EFST_MATK_2 = 387,
//	SI_FOOD_STR = 388,
//	SI_FOOD_INT = 389,
	SI_REFLECTDAMAGE = 390,
	SI_FORCEOFVANGUARD = 391,
// 392
	SI_AUTOSHADOWSPELL = 393,
	SI_SHADOWFORM = 394,
// 395
	SI_SHIELDSPELL_1 = 396,
	SI_SHIELDSPELL_2 = 397,
	SI_SHIELDSPELL_3 = 398,
	SI_BODYPAINTING = 399,
// 400
//	SI_DECREASEAGI_2 = 401,
	SI_PRESTIGE = 402,
	SI_INVISIBILITY = 403,
	SI_DEADLYINFECT = 404,
	SI_BANDING = 405,
// 406
	SI_INSPIRATION = 407,
	SI_MASQUERADE = 408, // [pakpil]
// 409
	SI_RAISINGDRAGON = 410,
// 411-412
	SI_LIGHTNINGWALK = 413,
	SI_EXPIATIO = 414,
// 415
	SI_BLACKCHAR = 416,	// Black char. I'm almost sure it's a Shadow Chaser skill, though it could be any of them. Maybe Body Painting? [LimitLine]
// 417-420
	SI_STRIPACCESSORY = 421,
	SI_MANHOLE = 422,
// 423-424
	SI_ENERGYGAIN = 425,
	SI_CHANGE = 426,
	SI_REVITALIZE = 427,
// 428
	SI_SWINGDANCE = 429,
	SI_SYMPHONYOFLOVERS = 430,
// 431-433
	SI_VOICEOFSIREN = 434,
	SI_DEEPSLEEP = 435,
	SI_SIRCLEOFNATURE = 436,
	SI_COLDSLOWER = 437,
	SI_GLOOMYDAY = 438,
	SI_SONGOFMANA = 439,
// 440
	SI_DANCEWITHWUG = 441,
	SI_RUSHWINDMILL = 442,
	SI_ECHOSONG = 443,
	SI_HARMONIZE = 444,
	SI_STRIKING = 445,
	SI_WARMER = 446,
	SI_MOONLITSERENADE = 447,
	SI_SATURDAYNIGHTFEVER = 448,
// 449
	SI_ANALYZE = 450,
	SI_LERADSDEW = 451,
	SI_MELODYOFSINK = 452,
	SI_WARCRYOFBEYOND = 453,
	SI_UNLIMITEDHUMMINGVOICE = 454,
// 455
	SI_SPELLBOOK_1 = 456,
	SI_SPELLBOOK_2 = 457,
// 458-460
	SI_CARTSBOOST = 461,
// 462
	SI_THORNTRAP = 463,
	SI_BLOODSUCKER = 464,
//  SI_LIKEBERSERK = 465,
//  466-471
	SI_SECRAMENT = 472,
//	SI_ASSUMPTIO_2 = 473,
};

enum wl_spheres {
	WLS_FIRE = 0x44,//0X54
	WLS_WIND,
	WLS_WATER,
	WLS_STONE,
};

// JOINTBEAT stackable ailments
#define BREAK_ANKLE    0x01 // MoveSpeed reduced by 50%
#define BREAK_WRIST    0x02 // ASPD reduced by 25%
#define BREAK_KNEE     0x04 // MoveSpeed reduced by 30%, ASPD reduced by 10%
#define BREAK_SHOULDER 0x08 // DEF reduced by 50%
#define BREAK_WAIST    0x10 // DEF reduced by 25%, ATK reduced by 25%
#define BREAK_NECK     0x20 // current attack does 2x damage, inflicts 'bleeding' for 30 seconds
#define BREAK_FLAGS    ( BREAK_ANKLE | BREAK_WRIST | BREAK_KNEE | BREAK_SHOULDER | BREAK_WAIST | BREAK_NECK )

extern int current_equip_item_index;
extern int current_equip_card_id;

extern int percentrefinery[5][MAX_REFINE+1]; //The last slot always has a 0% success chance [Skotlex]

//Mode definitions to clear up code reading. [Skotlex]
#define MD_CANMOVE 0x0001
#define MD_LOOTER 0x0002
#define MD_AGGRESSIVE 0x0004
#define MD_ASSIST 0x0008
#define MD_CASTSENSOR_IDLE 0x0010
#define MD_BOSS 0x0020
#define MD_PLANT 0x0040
#define MD_CANATTACK 0x0080
#define MD_DETECTOR 0x0100
#define MD_CASTSENSOR_CHASE 0x0200
#define MD_CHANGECHASE 0x0400
#define MD_ANGRY 0x0800
#define MD_CHANGETARGET_MELEE 0x1000
#define MD_CHANGETARGET_CHASE 0x2000
#define MD_TARGETWEAK 0x4000
#define MD_MASK 0xFFFF

//Status change option definitions (options are what makes status changes visible to chars
//who were not on your field of sight when it happened)

//opt1: Non stackable status changes.
enum {
	OPT1_STONE = 1, //Petrified
	OPT1_FREEZE,
	OPT1_STUN,
	OPT1_SLEEP,
	//Aegis uses OPT1 = 5 to identify undead enemies (which also grants them immunity to the other opt1 changes)
	OPT1_STONEWAIT=6, //Petrifying
	OPT1_BURNING
};

//opt2: Stackable status changes.
enum {
	OPT2_POISON       = 0x0001,
	OPT2_CURSE        = 0x0002,
	OPT2_SILENCE      = 0x0004,
	OPT2_SIGNUMCRUCIS = 0x0008,
	OPT2_BLIND        = 0x0010,
	OPT2_ANGELUS      = 0x0020,
	OPT2_BLEEDING     = 0x0040,
	OPT2_DPOISON      = 0x0080,
};

//opt3: (SHOW_EFST_*)
enum {
	OPT3_NORMAL           = 0x00000000,
	OPT3_QUICKEN          = 0x00000001,
	OPT3_OVERTHRUST       = 0x00000002,
	OPT3_ENERGYCOAT       = 0x00000004,
	OPT3_EXPLOSIONSPIRITS = 0x00000008,
	OPT3_STEELBODY        = 0x00000010,
	OPT3_BLADESTOP        = 0x00000020,
	OPT3_AURABLADE        = 0x00000040,
	OPT3_BERSERK          = 0x00000080,
	OPT3_LIGHTBLADE       = 0x00000100,
	OPT3_MOONLIT          = 0x00000200,
	OPT3_MARIONETTE       = 0x00000400,
	OPT3_ASSUMPTIO        = 0x00000800,
	OPT3_WARM             = 0x00001000,
	OPT3_KAITE            = 0x00002000,
	OPT3_BUNSIN           = 0x00004000,
	OPT3_SOULLINK         = 0x00008000,
	OPT3_UNDEAD           = 0x00010000,
	OPT3_CONTRACT         = 0x00020000,
};

enum {
	OPTION_NOTHING      = 0x00000000,
	OPTION_SIGHT        = 0x00000001,
	OPTION_HIDE         = 0x00000002,
	OPTION_CLOAK        = 0x00000004,
	OPTION_CART1        = 0x00000008,
	OPTION_FALCON       = 0x00000010,
	OPTION_RIDING       = 0x00000020, // it's used to mount gryphon too [pakpil]
	OPTION_INVISIBLE    = 0x00000040,
	OPTION_CART2        = 0x00000080,
	OPTION_CART3        = 0x00000100,
	OPTION_CART4        = 0x00000200,
	OPTION_CART5        = 0x00000400,
	OPTION_ORCISH       = 0x00000800,
	OPTION_WEDDING      = 0x00001000,
	OPTION_RUWACH       = 0x00002000,
	OPTION_CHASEWALK    = 0x00004000,
	OPTION_FLYING       = 0x00008000, //Note that clientside Flying and Xmas are 0x8000 for clients prior to 2007.
	OPTION_XMAS         = 0x00010000,
	OPTION_TRANSFORM    = 0x00020000,
	OPTION_SUMMER       = 0x00040000,
	OPTION_GREEN_DRAGON = 0x00080000,
	OPTION_WUG		    = 0x00100000,
	OPTION_RIDING_WUG   = 0x00200000,
	OPTION_MADO	        = 0x00400000,
	OPTION_BLACK_DRAGON = 0x00800000,
	OPTION_WHITE_DRAGON = 0x01000000,
	OPTION_BLUE_DRAGON  = 0x02000000,
	OPTION_RED_DRAGON   = 0x04000000,
};

#define OPTION_CART (OPTION_CART1|OPTION_CART2|OPTION_CART3|OPTION_CART4|OPTION_CART5)
#define OPTION_RIDING_DRAGON (OPTION_GREEN_DRAGON|OPTION_BLACK_DRAGON|OPTION_WHITE_DRAGON|OPTION_BLUE_DRAGON|OPTION_RED_DRAGON)

#define OPTION_MASK ~0x40

//Defines for the manner system [Skotlex]
#define MANNER_NOCHAT 0x01
#define MANNER_NOSKILL 0x02
#define MANNER_NOCOMMAND 0x04
#define MANNER_NOITEM 0x08
#define MANNER_NOROOM 0x10

//Define flags for the status_calc_bl function. [Skotlex]
enum scb_flag
{
	SCB_NONE    = 0x00000000,
	SCB_BASE    = 0x00000001,
	SCB_MAXHP   = 0x00000002,
	SCB_MAXSP   = 0x00000004,
	SCB_STR     = 0x00000008,
	SCB_AGI     = 0x00000010,
	SCB_VIT     = 0x00000020,
	SCB_INT     = 0x00000040,
	SCB_DEX     = 0x00000080,
	SCB_LUK     = 0x00000100,
	SCB_BATK    = 0x00000200,
	SCB_WATK    = 0x00000400,
	SCB_MATK    = 0x00000800,
	SCB_HIT     = 0x00001000,
	SCB_FLEE    = 0x00002000,
	SCB_DEF     = 0x00004000,
	SCB_DEF2    = 0x00008000,
	SCB_MDEF    = 0x00010000,
	SCB_MDEF2   = 0x00020000,
	SCB_SPEED   = 0x00040000,
	SCB_ASPD    = 0x00080000,
	SCB_DSPD    = 0x00100000,
	SCB_CRI     = 0x00200000,
	SCB_FLEE2   = 0x00400000,
	SCB_ATK_ELE = 0x00800000,
	SCB_DEF_ELE = 0x01000000,
	SCB_MODE    = 0x02000000,
	SCB_SIZE    = 0x04000000,
	SCB_RACE    = 0x08000000,
	SCB_RANGE   = 0x10000000,
	SCB_REGEN   = 0x20000000,
	SCB_DYE     = 0x40000000, // force cloth-dye change to 0 to avoid client crashes.

	SCB_BATTLE  = 0x3FFFFFFE,
	SCB_ALL     = 0x3FFFFFFF
};

//Define to determine who gets HP/SP consumed on doing skills/etc. [Skotlex]
#define BL_CONSUME (BL_PC|BL_HOM|BL_MER)
//Define to determine who has regen
#define BL_REGEN (BL_PC|BL_HOM|BL_MER)


//Basic damage info of a weapon
//Required because players have two of these, one in status_data
//and another for their left hand weapon.
struct weapon_atk {
	unsigned short atk, atk2;
	unsigned short range;
	unsigned char ele;
};


//For holding basic status (which can be modified by status changes)
struct status_data {
	unsigned int
		hp, sp,
		max_hp, max_sp,
		mech_hp, mech_sp,
		mado_hp, mado_sp;
	unsigned short
		str, agi, vit, int_, dex, luk,
		batk,
		matk_min, matk_max,
		speed,
		amotion, adelay, dmotion,
		mode;
	short 
		hit, flee, cri, flee2,
		def2, mdef2,
		aspd_rate;
	unsigned char
		def_ele, ele_lv,
		size, race;
	signed char
		def, mdef;
	int mado_heat;	// Overheat status. [LimitLine]
	struct weapon_atk rhw, lhw; //Right Hand/Left Hand Weapon.
};

//Additional regen data that only players have.
struct regen_data_sub {
	unsigned short
		hp,sp;

	//tick accumulation before healing.
	struct {
		unsigned int hp,sp;
	} tick;
	
	//Regen rates (where every 1 means +100% regen)
	struct {
		unsigned char hp,sp;
	} rate;
};

struct regen_data {

	unsigned short flag; //Marks what stuff you may heal or not.
	unsigned short
		hp,sp,shp,ssp;

	//tick accumulation before healing.
	struct {
		unsigned int hp,sp,shp,ssp;
	} tick;
	
	//Regen rates (where every 1 means +100% regen)
	struct {
		unsigned char
		hp,sp,shp,ssp;
	} rate;
	
	struct {
		unsigned walk:1; //Can you regen even when walking?
		unsigned gc:1;	//Tags when you should have double regen due to GVG castle
		unsigned overweight :2; //overweight state (1: 50%, 2: 90%)
		unsigned block :2; //Block regen flag (1: Hp, 2: Sp)
	} state;

	//skill-regen, sitting-skill-regen (since not all chars with regen need it)
	struct regen_data_sub *sregen, *ssregen;
};

struct status_change_entry {
	int timer;
	int val1,val2,val3,val4;
};

struct status_change {
	unsigned int option;// effect state (bitfield)
	unsigned int opt3;// skill state (bitfield)
	unsigned short opt1;// body state
	unsigned short opt2;// health state (bitfield)
	unsigned char count;
	//TODO: See if it is possible to implement the following SC's without requiring extra parameters while the SC is inactive.
	unsigned char jb_flag; //Joint Beat type flag
	unsigned short mp_matk_min, mp_matk_max; //Previous matk min/max for ground spells (Amplify magic power)
	int sg_id; //ID of the previous Storm gust that hit you
	unsigned char sg_counter; //Storm gust counter (previous hits from storm gust)
	int cl_tid;	//ID of the next picked target for Chain Lightning. [LimitLine]
	int cl_counter;	//Chain Lightning counter. [LimitLine]
	int cl_counter2;//Chain Lightning counter (for the three-hit countdown when there's nobody in range). [LimitLine]
	int comet_x, comet_y;	// X and Y coordinates of the center of the comet that hit you. [LimitLine]
	struct status_change_entry *data[SC_MAX];
};

// for looking up associated data
sc_type status_skill2sc(int skill);
int status_sc2skill(sc_type sc);

int status_damage(struct block_list *src,struct block_list *target,int hp,int sp, int walkdelay, int flag);
//Define for standard HP damage attacks.
#define status_fix_damage(src, target, hp, walkdelay) status_damage(src, target, hp, 0, walkdelay, 0)
//Define for standard HP/SP damage triggers.
#define status_zap(bl, hp, sp) status_damage(NULL, bl, hp, sp, 0, 1)
//Define for standard HP/SP skill-related cost triggers (mobs require no HP/SP to use skills)
int status_charge(struct block_list* bl, int hp, int sp);
int status_percent_change(struct block_list *src,struct block_list *target,signed char hp_rate, signed char sp_rate, int flag);
//Easier handling of status_percent_change
#define status_percent_heal(bl, hp_rate, sp_rate) status_percent_change(NULL, bl, -(hp_rate), -(sp_rate), 0)
#define status_percent_damage(src, target, hp_rate, sp_rate, kill) status_percent_change(src, target, hp_rate, sp_rate, (kill)?1:2)
//Instant kill with no drops/exp/etc
#define status_kill(bl) status_percent_damage(NULL, bl, 100, 0, true)
//Used to set the hp/sp of an object to an absolute value (can't kill)
int status_set_hp(struct block_list *bl, unsigned int hp, int flag);
int status_set_sp(struct block_list *bl, unsigned int sp, int flag);
int status_heal(struct block_list *bl,int hp,int sp, int flag);
int status_revive(struct block_list *bl, unsigned char per_hp, unsigned char per_sp);

//Define for copying a status_data structure from b to a, without overwriting current Hp and Sp
#define status_cpy(a, b) \
	memcpy(&((a)->max_hp), &((b)->max_hp), sizeof(struct status_data)-(sizeof((a)->hp)+sizeof((a)->sp)))

struct regen_data *status_get_regen_data(struct block_list *bl);
struct status_data *status_get_status_data(struct block_list *bl);
struct status_data *status_get_base_status(struct block_list *bl);
const char * status_get_name(struct block_list *bl);
int status_get_class(struct block_list *bl);
int status_get_lv(struct block_list *bl);
#define status_get_range(bl) status_get_status_data(bl)->rhw.range
#define status_get_hp(bl) status_get_status_data(bl)->hp
#define status_get_max_hp(bl) status_get_status_data(bl)->max_hp
#define status_get_sp(bl) status_get_status_data(bl)->sp
#define status_get_max_sp(bl) status_get_status_data(bl)->max_sp
#define status_get_str(bl) status_get_status_data(bl)->str
#define status_get_agi(bl) status_get_status_data(bl)->agi
#define status_get_vit(bl) status_get_status_data(bl)->vit
#define status_get_int(bl) status_get_status_data(bl)->int_
#define status_get_dex(bl) status_get_status_data(bl)->dex
#define status_get_luk(bl) status_get_status_data(bl)->luk
#define status_get_hit(bl) status_get_status_data(bl)->hit
#define status_get_flee(bl) status_get_status_data(bl)->flee
signed char status_get_def(struct block_list *bl);
#define status_get_mdef(bl) status_get_status_data(bl)->mdef
#define status_get_flee2(bl) status_get_status_data(bl)->flee2
#define status_get_def2(bl) status_get_status_data(bl)->def2
#define status_get_mdef2(bl) status_get_status_data(bl)->mdef2
#define status_get_critical(bl)  status_get_status_data(bl)->cri
#define status_get_batk(bl) status_get_status_data(bl)->batk
#define status_get_watk(bl) status_get_status_data(bl)->rhw.atk
#define status_get_watk2(bl) status_get_status_data(bl)->rhw.atk2
#define status_get_matk_max(bl) status_get_status_data(bl)->matk_max
#define status_get_matk_min(bl) status_get_status_data(bl)->matk_min
#define status_get_lwatk(bl) status_get_status_data(bl)->lhw.atk
#define status_get_lwatk2(bl) status_get_status_data(bl)->lhw.atk2
unsigned short status_get_speed(struct block_list *bl);
#define status_get_adelay(bl) status_get_status_data(bl)->adelay
#define status_get_amotion(bl) status_get_status_data(bl)->amotion
#define status_get_dmotion(bl) status_get_status_data(bl)->dmotion
#define status_get_element(bl) status_get_status_data(bl)->def_ele
#define status_get_element_level(bl) status_get_status_data(bl)->ele_lv
unsigned char status_calc_attack_element(struct block_list *bl, struct status_change *sc, int element);
#define status_get_attack_sc_element(bl, sc) status_calc_attack_element(bl, sc, 0)
#define status_get_attack_element(bl) status_get_status_data(bl)->rhw.ele
#define status_get_attack_lelement(bl) status_get_status_data(bl)->lhw.ele
#define status_get_race(bl) status_get_status_data(bl)->race
#define status_get_size(bl) status_get_status_data(bl)->size
#define status_get_mode(bl) status_get_status_data(bl)->mode
int status_get_party_id(struct block_list *bl);
int status_get_guild_id(struct block_list *bl);
int status_get_emblem_id(struct block_list *bl);
int status_get_mexp(struct block_list *bl);
int status_get_race2(struct block_list *bl);

struct view_data *status_get_viewdata(struct block_list *bl);
void status_set_viewdata(struct block_list *bl, int class_);
void status_change_init(struct block_list *bl);
struct status_change *status_get_sc(struct block_list *bl);

int status_isdead(struct block_list *bl);
int status_isimmune(struct block_list *bl);

int status_get_sc_def(struct block_list *bl, enum sc_type type, int rate, int tick, int flag);
//Short version, receives rate in 1->100 range, and does not uses a flag setting.
#define sc_start(bl, type, rate, val1, tick) status_change_start(bl,type,100*(rate),val1,0,0,0,tick,0)
#define sc_start2(bl, type, rate, val1, val2, tick) status_change_start(bl,type,100*(rate),val1,val2,0,0,tick,0)
#define sc_start4(bl, type, rate, val1, val2, val3, val4, tick) status_change_start(bl,type,100*(rate),val1,val2,val3,val4,tick,0)

int status_change_start(struct block_list* bl,enum sc_type type,int rate,int val1,int val2,int val3,int val4,int tick,int flag);
int status_change_end(struct block_list* bl, enum sc_type type, int tid);
int kaahi_heal_timer(int tid, unsigned int tick, int id, intptr data);
int status_change_timer(int tid, unsigned int tick, int id, intptr data);
int status_change_timer_sub(struct block_list* bl, va_list ap);
int status_change_clear(struct block_list* bl, int type);
int status_change_clear_buffs(struct block_list* bl, int type);
int status_change_spread( struct block_list *src, struct block_list *bl );

#define status_calc_bl(bl, flag) status_calc_bl_(bl, flag, false)
#define status_calc_mob(md, first) status_calc_bl_(&(md)->bl, SCB_ALL, first)
#define status_calc_pet(pd, first) status_calc_bl_(&(pd)->bl, SCB_ALL, first)
#define status_calc_pc(sd, first) status_calc_bl_(&(sd)->bl, SCB_ALL, first)
#define status_calc_homunculus(hd, first) status_calc_bl_(&(hd)->bl, SCB_ALL, first)
#define status_calc_mercenary(md, first) status_calc_bl_(&(md)->bl, SCB_ALL, first)

void status_calc_bl_(struct block_list *bl, enum scb_flag flag, bool first);
int status_calc_mob_(struct mob_data* md, bool first);
int status_calc_pet_(struct pet_data* pd, bool first);
int status_calc_pc_(struct map_session_data* sd, bool first);
int status_calc_homunculus_(struct homun_data *hd, bool first);
int status_calc_mercenary_(struct mercenary_data *md, bool first);

void status_calc_misc(struct block_list *bl, struct status_data *status, int level);
void status_calc_regen(struct block_list *bl, struct status_data *status, struct regen_data *regen);
void status_calc_regen_rate(struct block_list *bl, struct regen_data *regen, struct status_change *sc);

int status_getrefinebonus(int lv,int type);
int status_check_skilluse(struct block_list *src, struct block_list *target, int skill_num, int flag); // [Skotlex]
int status_check_visibility(struct block_list *src, struct block_list *target); //[Skotlex]

int status_readdb(void);
int do_init_status(void);
void do_final_status(void);

#endif /* _STATUS_H_ */
