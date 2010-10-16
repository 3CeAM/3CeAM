#
# Table structure for table `item_db2`
#

DROP TABLE IF EXISTS `item_db2`;
CREATE TABLE `item_db2` (
  `id` smallint(5) unsigned NOT NULL default '0',
  `name_english` varchar(50) NOT NULL default '',
  `name_japanese` varchar(50) NOT NULL default '',
  `type` tinyint(2) unsigned NOT NULL default '0',
  `price_buy` mediumint(10) unsigned default NULL,
  `price_sell` mediumint(10) unsigned default NULL,
  `weight` smallint(5) unsigned NOT NULL default '0',
  `attack` smallint(3) unsigned default NULL,
  `defence` tinyint(3) unsigned default NULL,
  `range` tinyint(2) unsigned default NULL,
  `slots` tinyint(2) unsigned default NULL,
  `equip_jobs` int(12) unsigned default NULL,
  `equip_upper` tinyint(8) unsigned default NULL,
  `equip_genders` tinyint(2) unsigned default NULL,
  `equip_locations` smallint(4) unsigned default NULL,
  `weapon_level` tinyint(2) unsigned default NULL,
  `equip_level` tinyint(3) unsigned default NULL,
  `refineable` tinyint(1) unsigned default NULL,
  `view` smallint(3) unsigned default NULL,
  `script` text,
  `equip_script` text,
  `unequip_script` text,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM;

#  Items Additional Database - Custom Items go here
# 
#  Structure of Database:
# REPLACE INTO `item_db2` VALUES ('ID','Name','Name','Type','Price','Sell','Weight','ATK','DEF','Range','Slot','Job','Upper','Gender','Loc','wLV','eLV','Refineable','View','Script','OnEquip_Script','OnUnequip_Script');
# 
#  THQ Quest Items
# =============================================================
# REPLACE INTO `item_db2` VALUES (7950,'THG_Membership','THG Membership',3,NULL,10,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
# REPLACE INTO `item_db2` VALUES (7951,'Token_Bag','Token Bag',3,NULL,10,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
# REPLACE INTO `item_db2` VALUES (1998,'Jeramiah\'s_Jur','Jeramiah\'s Jur',3,NULL,10,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
# REPLACE INTO `item_db2` VALUES (1999,'Zed\'s_Staff','Zed\'s Staff',3,NULL,10,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

#  Official Event Items that had their Effects removed after the event was completed
# REPLACE INTO `item_db2` VALUES (585,'Wurst','Brusti',11,2,NULL,40,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'itemheal rand(15,20),0; itemskill \"PR_MAGNIFICAT\",3;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (679,'Gold_Pill','Pilule',0,5000,NULL,300,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'percentheal 50,50;',NULL,NULL);

# REPLACE INTO `item_db2` VALUES (2681,'Republic_Ring','Republic Anniversary Ring',5,20,NULL,100,NULL,0,NULL,0,0xFFFFFFFF,7,2,136,NULL,0,0,0,'bonus bAllStats,3;',NULL,NULL);

# REPLACE INTO `item_db2` VALUES (5134,'Pumpkin_Hat','Pumpkin-Head',5,20,NULL,200,NULL,2,NULL,0,0xFFFFFFFF,7,2,256,NULL,0,1,206,'bonus2 bSubRace,RC_Demon,5;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (5136,'Santa\'s_Hat_','Louise\'s Santa Hat',5,20,NULL,100,NULL,3,NULL,0,0xFFFFFFFF,7,2,256,NULL,0,1,20,'bonus bMdef,1; bonus bLuk,1; bonus3 bAutoSpellWhenHit,\"AL_HEAL\",3,50; bonus3 bAutoSpellWhenHit,\"AL_BLESSING\",10,50;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (5145,'Carnival_Joker_Jester','Carnival Jester',5,10,NULL,100,NULL,0,NULL,0,0xFFFFFFFF,7,2,256,NULL,0,1,89,'bonus bAllStats,3;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (5147,'Baseball_Cap','Baseball Cap',5,0,NULL,200,NULL,3,NULL,0,0xFFFFFFFF,7,2,256,NULL,0,1,216,'bonus2 bExpAddRace,RC_Boss,50; bonus2 bExpAddRace,RC_NonBoss,50;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (5201,'Party_Hat_B','2nd Anniversary Party Hat',5,20,NULL,300,NULL,3,NULL,0,0xFFFFFFFF,7,2,256,NULL,0,1,144,'bonus bAllStats,3;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (5202,'Pumpkin_Hat_','Pumpkin Hat',5,20,NULL,200,NULL,2,NULL,0,0xFFFFFFFF,7,2,256,NULL,0,1,206,'bonus bAllStats,2; bonus2 bSubRace,RC_Demon,5; bonus3 bAddMonsterDropItem,529,RC_DemiHuman,1500;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (5204,'Event_Pierrot_Nose','Rudolf\'s Red Nose',5,20,NULL,100,NULL,0,NULL,0,0xFFFFFFFF,7,2,1,NULL,0,0,49,'bonus2 bResEff,Eff_Blind,3000; bonus2 bAddMonsterDropItem,12130,30;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (5264,'Aussie_Flag_Hat','Australian Flag Hat',5,20,NULL,500,NULL,4,NULL,0,0xFFFFFFFF,7,2,256,NULL,0,1,304,'bonus bAllStats,2;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (5356,'Pumpkin_Hat_H','Pumpkin Hat',5,20,NULL,200,NULL,2,NULL,0,0xFFFFFFFF,7,2,256,NULL,0,1,206,'bonus bAllStats,2; bonus2 bSubRace,RC_Demon,5; bonus2 bMagicAddRace,RC_Demon,5;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (5811,'Santa_Beard','Santa Beard',5,20,NULL,100,NULL,5,NULL,0,0xFFFFFFFF,7,2,1,NULL,0,0,25,'bonus2 bSubRace,RC_Brute,5;',NULL,NULL);

# REPLACE INTO `item_db2` VALUES (11702,'Moon_Cookie','Moon Cookie',11,0,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_end SC_Poison; sc_end SC_Silence; sc_end SC_Blind; sc_end SC_Confusion; sc_end SC_Curse; sc_end SC_Hallucination; itemskill \"AL_BLESSING\",7;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (12131,'Lucky_Potion','Lucky Potion',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_LUKFood,180000,15;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (12143,'Red_Can','Red Can',2,50000,NULL,300,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'percentheal 25,25;',NULL,NULL);
# Event effect: Summon monster? Probably Rice_Cake. x_x
# REPLACE INTO `item_db2` VALUES (12199,'Rice_Scroll','Rice Scroll',2,0,NULL,0,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
# REPLACE INTO `item_db2` VALUES (12200,'Event_Cake','Event Cake',2,20,NULL,50,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'itemskill \"PR_MAGNIFICAT\",3;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (12238,'New_Year_Rice_Cake_1','New Year Rice Cake',0,20,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'percentheal 20,15; sc_start SC_STRFood,1200000,3; sc_start SC_INTFood,1200000,3; sc_start SC_LUKFood,1200000,3; sc_start SC_SpeedUp1,5000,0;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (12239,'New_Year_Rice_Cake_2','New Year Rice Cake',0,20,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'percentheal 20,15; sc_start SC_DEXFood,1200000,3; sc_start SC_AGIFood,1200000,3; sc_start SC_VITFood,1200000,3; sc_start SC_SpeedUp1,5000,0;',NULL,NULL);

#  iRO St. Patrick's Day Event 2008
# =============================================================
# REPLACE INTO `item_db2` VALUES (12715,'Black_Treasure_Chest','Black Treasure Chest',2,0,NULL,200,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'callfunc \"F_08stpattyseventbox\";',NULL,NULL);

#  iRO Valentine's Day Event 2009
# =============================================================
# REPLACE INTO `item_db2` VALUES (12742,'Valentine_Gift_Box_M','Valentine Gift Box',2,10,NULL,0,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'getitem 7946,1;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (12743,'Valentine_Gift_Box_F','Valentine Gift Box',2,10,NULL,0,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'getitem 7947,1;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (12744,'Chocolate_Box','Chocolate Box',2,10,NULL,0,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'getitem 558,1;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (14466,'Valentine\'s_Emblem_Box','Valentine\'s Emblem Box',2,10,NULL,0,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'getitem 5817,1;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (7946,'Gold_Ring_Of_Valentine','Gold Ring Of Valentine',3,10,NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
# REPLACE INTO `item_db2` VALUES (7947,'Silver_Ring_Of_Valentine','Silver Ring Of Valentine',3,10,NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
# REPLACE INTO `item_db2` VALUES (7948,'Box','Box',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
# REPLACE INTO `item_db2` VALUES (5817,'Valentine\'s_Emblem','Valentine\'s Emblem',5,10,NULL,0,NULL,3,NULL,0,0xFFFFFFFF,7,2,136,NULL,0,0,0,'bonus bAtkRate,3; bonus bMatkRate,3; bonus bAllStats,2; bonus bFlee,10; bonus bAspd,1; bonus bMdef,3; bonus2 bSkillAtk,\"AL_HEAL\",10; bonus2 bSkillHeal,\"AL_HEAL\",10; bonus2 bSkillHeal,\"AM_POTIONPITCHER\",10; bonus2 bAddItemHealRate,IG_Potion,10;',NULL,NULL);

#  iRO Halloween Event 2009
# =============================================================
# REPLACE INTO `item_db2` VALUES (5668,'Weird_Pumpkin_Hat','Weird Pumpkin Hat',5,20,NULL,0,NULL,5,NULL,0,0xFFFFFFFF,7,2,256,NULL,0,1,206,'bonus bMdef,5; bonus2 bAddMonsterDropItem,12192,2500;',NULL,NULL);
# REPLACE INTO `item_db2` VALUES (6298,'Crushed_Pumpkin','Crushed Pumpkin',3,0,NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
# REPLACE INTO `item_db2` VALUES (6299,'Worn_Fabric','Worn Fabric',3,0,NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

#  Old Tuxedo and Wedding Dress, will display the outfit when worn.
# ==================================================================
# REPLACE INTO `item_db2` VALUES (2338,'Wedding_Dress','Wedding Dress',5,43000,NULL,500,NULL,0,NULL,0,0xFFFFFFFE,7,0,16,NULL,0,1,0,NULL,'setoption Option_Wedding,1;','setoption Option_Wedding,0;');
# REPLACE INTO `item_db2` VALUES (7170,'Tuxedo','Tuxedo',5,43000,NULL,10,NULL,0,NULL,0,0xFFFFFFFE,7,1,16,NULL,0,1,0,NULL,'setoption Option_Wedding,1;','setoption Option_Wedding,0;');

# ========================================================================
# ================ 3rd Job Related Item database =========================
# ========================================================================

# 3rd Class Weapons
# =============================================================
REPLACE INTO `item_db2` VALUES (1191,'Alcabringer','Alcabringer',4,20,NULL,3400,280,NULL,1,0,0x00004080,8,2,34,3,100,1,3,'','','');
REPLACE INTO `item_db2` VALUES (1287,'Dulga','Dulga',4,20,NULL,1200,190,NULL,1,1,0x00001000,8,2,34,3,100,1,16,'','','');
REPLACE INTO `item_db2` VALUES (1649,'Lapine_Staff','Lapine Staff',4,20,NULL,500,30,NULL,1,0,0x00018300,8,2,2,3,100,1,10,'bonus bMatkRate,30; bonus bFixCastRate,-getrefine();','','');
REPLACE INTO `item_db2` VALUES (1746,'Elven_Bow','Elven Bow',4,20,NULL,1500,160,NULL,5,1,0x00080800,8,2,34,3,100,1,11,'','','');
REPLACE INTO `item_db2` VALUES (16003,'Kalga_Mace','Kalga Mace',4,20,NULL,1500,175,NULL,1,2,0x0004C580,8,2,2,3,100,1,8,'','','');
REPLACE INTO `item_db2` VALUES (1549,'Pile_Bunker','Pile Bunker',4,10000,NULL,3000,450,NULL,1,0,0x00000400,8,2,2,3,99,0,8,'','','');
REPLACE INTO `item_db2` VALUES (2139,'Flame_Thrower','Flame Thrower',5,20000,NULL,2000,NULL,5,NULL,0,0x00000400,8,2,32,NULL,99,0,5,'','','');
REPLACE INTO `item_db2` VALUES (2140,'Energy_Rune_Guard','Energy Rune Guard',5,20,NULL,10,NULL,1,NULL,1,0xFFFFFFFF,7,2,32,NULL,70,1,5,'bonus bMaxSPrate,2;','','');

# Mechanic Mado Accessorys
# =============================================================
REPLACE INTO `item_db2` VALUES (2800,'Accelerator','Accelerator',5,100000,NULL,100,NULL,0,NULL,1,0x00000400,8,2,136,NULL,99,0,0,'bonus bAgi,2;','','');
REPLACE INTO `item_db2` VALUES (2801,'Hover_Booster','Hover Booster',5,100000,NULL,100,NULL,0,NULL,1,0x00000400,8,2,136,NULL,99,0,0,'bonus bAgi,1;','','');
REPLACE INTO `item_db2` VALUES (2802,'Self_Destruct_Device','Self Destruct Device',5,500000,NULL,1000,NULL,0,NULL,1,0x00000400,8,2,136,NULL,99,0,0,'','','');
REPLACE INTO `item_db2` VALUES (2803,'Shape_Shifter','Shape Shifter',5,100000,NULL,500,NULL,0,NULL,1,0x00000400,8,2,136,NULL,99,0,0,'bonus bInt,3;','','');
REPLACE INTO `item_db2` VALUES (2804,'Cooling_System','Cooling System',5,100000,NULL,2500,NULL,0,NULL,1,0x00000400,8,2,136,NULL,99,0,0,'bonus bDex,1;','','');
REPLACE INTO `item_db2` VALUES (2805,'Magnetic_Field_Generator','Magnetic Field Generator',5,100000,NULL,6000,NULL,0,NULL,1,0x00000400,8,2,136,NULL,99,0,0,'bonus bDex,1;','','');
REPLACE INTO `item_db2` VALUES (2806,'Barrier_Generator','Barrier Generator',5,150000,NULL,8000,NULL,3,NULL,1,0x00000400,8,2,136,NULL,99,0,0,'bonus bDex,1;','','');
REPLACE INTO `item_db2` VALUES (2807,'Repair_Kit','Repair Kit',5,200000,NULL,400,NULL,0,NULL,1,0x00000400,8,2,136,NULL,99,0,0,'','','');
REPLACE INTO `item_db2` VALUES (2808,'Optical_Camouflage_Generator','Optical Camouflage Generator',5,250000,NULL,1000,NULL,0,NULL,1,0x00000400,8,2,136,NULL,99,0,0,'bonus bAgi,3;','','');
REPLACE INTO `item_db2` VALUES (2809,'Advanced_Cooling_System','Advanced Cooling System',5,20,NULL,10,NULL,0,NULL,1,0x00000400,8,2,136,NULL,99,0,0,'bonus bDex,1;','','');
REPLACE INTO `item_db2` VALUES (2810,'Special_Cooling_System','Special Cooling System',5,20,NULL,10,NULL,0,NULL,1,0x00000400,8,2,136,NULL,99,0,0,'bonus bDex,1;','','');


# Shadow Chaser Paint And Brushes
# =============================================================
REPLACE INTO `item_db2` VALUES (6120,'Face_Paint','Face Paint',3,120,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6121,'Makeup_Brush','Makeup Brush',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6122,'Paint_Brush','Paint Brush',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6123,'Surface_Paint','Surface Paint',3,200,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Guillotine Cross Poison Antidote
# =============================================================
REPLACE INTO `item_db2` VALUES (6128,'Antidote','Antidote',3,1,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Wanderer / Minstrel Skill Required Item. 
# =============================================================
REPLACE INTO `item_db2` VALUES (6144,'Painful_Tear','Painful Tear',3,1000,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Mechanic Mado Gear Skill Required Items
# =============================================================
REPLACE INTO `item_db2` VALUES (6145,'Vulcan_Bullet','Vulcan Bullet',3,10,NULL,3,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6146,'Mado_Gear_Fuel','Mado Gear Fuel',3,300,NULL,30,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6147,'Liquid_Cold_Bullet','Liquid Cold Bullet',3,100,NULL,30,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# =============================================================
REPLACE INTO `item_db2` VALUES (6150,'House_Key','House Key',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6151,'Huge_Bradium','Huge Bradium',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6152,'Crystal_Shine','Crystal Shine',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6153,'Special_Exange_Ticket','Special Exange Ticket',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6154,'Hisui\'s_Horn','Hisui\'s Horn',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6156,'Payment_Waiting_Report','Payment Waiting Report',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6157,'Poring_Exange_Ticket','Poring Exange Ticket',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6158,'Drops_Exange_Ticket','Drops Exange Ticket',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6159,'Poporing_Exange_Ticket','Poporing Exange Ticket',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6160,'Lunatic_Exange_Ticket','Lunatic Exange Ticket',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6186,'Monkey_Wrench','Monkey Wrench',3,500,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Warlock's Magic Spellbooks
# =============================================================
REPLACE INTO `item_db2` VALUES (6189,'Magic_Book_(Fire_Bolt)','Magic Book (Fire Bolt)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6190,'Magic_Book_(Cold_Bolt)','Magic Book (Cold Bolt)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6191,'Magic_Book_(Lightning_Bolt)','Magic Book (Lightning Bolt)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6192,'Magic_Book_(Storm_Gust)','Magic Book (Storm Gust)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6193,'Magic_Book_(Lord_Of_Vermillion)','Magic Book (Lord Of Vermillion)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6194,'Magic_Book_(Meteor_Storm)','Magic Book (Meteor Storm)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6195,'Magic_Book_(Comet)','Magic Book (Comet)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6196,'Magic_Book_(Tetra_Vortex)','Magic Book (Tetra Vortex)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6197,'Magic_Book_(Thunder_Storm)','Magic Book (Thunder Storm)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6198,'Magic_Book_(Jupitel_Thunder)','Magic Book (Jupitel Thunder)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6199,'Magic_Book_(Water_Ball)','Magic Book (Water_Ball)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6200,'Magic_Book_(Heaven\'s_Drive)','Magic Book (Heaven\'s Drive)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6201,'Magic_Book_(Earth_Spike)','Magic Book (Earth Spike)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6202,'Magic_Book_(Earth_Strain)','Magic Book (Earth Strain)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6203,'Magic_Book_(Chain_Lightning)','Magic Book (Chain Lightning)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6204,'Magic_Book_(Crimson_Rock)','Magic Book (Crimson_Rock)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6205,'Magic_Book_(Drain_Life)','Magic Book (Drain Life)',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');


# Genetic Skill Required Items
# =============================================================
REPLACE INTO `item_db2` VALUES (6210,'Thorny_Seed','Thorny Seed',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6211,'Bloodsucker_Seed','Bloodsucker Seed',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6212,'Bomb_Mushroom_Spores','Bomb Mushroom Spores',3,20,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6213,'Explosive_Powder','Explosive Powder',3,500,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6214,'Smokescreen_Powder','Smokescreen Powder',3,500,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6215,'Tear_Gas','Tear Gas',3,500,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6216,'Oil_Bottle','Oil Bottle',3,1000,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6217,'Mandragora_Flowerpot','Mandragora Flowerpot',3,2000,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Genetic Item Creation Ingredients
# =============================================================
REPLACE INTO `item_db2` VALUES (6244,'Dark_Powder','Dark Powder',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6245,'Black_Powder','Black Powder',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6246,'Yellow_Powder','Yellow Powder',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6247,'White_Powder','White Powder',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6248,'Chowder_Pot','Chowder Pot',3,600,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6249,'Savage_Meat','Savage Meat',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6250,'Iron_Cooking_Skewer','Iron Cooking Skewer',3,300,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6251,'Black_Charcoal','Black Charcoal',3,300,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6252,'Wolf\'s_Blood','Wolf\'s Blood',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6253,'Cold_Ice','Cold Ice',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6254,'Seasoned_Tough_Meat','Seasoned Tough Meat',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6255,'Large_Pan','Large Pan',3,500,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6256,'Powdered_Ice','Powdered Ice',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6257,'Ice_Crystal','Ice Crystal',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6258,'Comodo_Tropical_Fruit','Comodo Tropical Fruit',3,800,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6259,'Drosera_Feeler','Drosera Feeler',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6260,'Petite_Tail','Petite Tail',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6261,'Fine_Noodles','Fine Noodles',3,500,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6262,'Cold_Broth','Cold Broth',3,400,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6263,'Coconut','Coconut',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6264,'Melon','Melon',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6265,'Pineapple','Pineapple',3,100,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Genetic Guide's And Manurals
# =============================================================
REPLACE INTO `item_db2` VALUES (6279,'Apple_Bomb_Guidebook','Apple Bomb Guidebook',3,1000,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6280,'Pineapple_Bomb_Guidebook','Pineapple Bomb Guidebook',3,1000,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6281,'Coconut_Bomb_Guidebook','Coconut Bomb Guidebook',3,1000,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6282,'Melon_Bomb_Guidebook','Melon Bomb Guidebook',3,1000,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6283,'Banana_Bomb_Guidebook','Banana Bomb Guidebook',3,1000,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6284,'Plant_Genetic_Cultivation_Guide','Plant Genetic Cultivation Guide',3,1500,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6285,'Improved_Potion_Creation_Manual','Improved Potion Creation Manual',3,1000,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Genetic's 'Throwing Kit'
# =============================================================
REPLACE INTO `item_db2` VALUES (6297,'Throwing_Bottle','Throwing Bottle',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Sorcerer Elemental Points
# =============================================================
REPLACE INTO `item_db2` VALUES (6360,'Scarlet_Point','Scarlet Point',3,100,NULL,5,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6361,'Indigo_Point','Indigo Point',3,100,NULL,5,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6362,'Yellow_Wish_Point','Yellow Wish Point',3,100,NULL,5,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6363,'Lime_Green_Point','Lime Green Point',3,100,NULL,5,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Guillotine Cross Poison Ingredients
# =============================================================
REPLACE INTO `item_db2` VALUES (7931,'Poison_Kit','Poison Manufacture Kit',3,1,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (7932,'Poison_Herb_Nerium','Nerium',3,1,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (7933,'Poison_Herb_Rantana','Lantana',3,1,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (7934,'Poison_Herb_Makulata','Makulrata',3,1,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (7935,'Poison_Herb_Seratum','Celatom',3,1,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (7936,'Poison_Herb_Scopolia','Scoforia',3,1,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (7937,'Poison_Herb_Amoena','Amoena',3,1,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Rune Knight Rune Stone Ingredients
# =============================================================
REPLACE INTO `item_db2` VALUES (7938,'Light_Granule','Light Partacle',3,1000,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (7939,'Elder_Branch','Elder\'s Branch',3,1000,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (7940,'Special_Alloy_Trap','Special Alloy Trap',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (11022,'Mix_Cooking_Ingredient_Book_1','Mix Cooking Ingredient Book 1',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (11023,'Vitality_Boost_Research_Book','Vitality Boost Research Book',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (11024,'Energy_Drink_Formula','Energy Drink Formula',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (11513,'Cough_Drop','Cough Drop',3,200,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (11518,'Cure_Free','Cure Free',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'itemheal rand(1000,1200),0; sc_end SC_Silence; sc_end SC_Bleeding; sc_end SC_Curse;','','');
REPLACE INTO `item_db2` VALUES (12333,'Ancilla','Ancilla',0,100,NULL,10,NULL,NULL,NULL,NULL,0x00000100,8,2,NULL,NULL,NULL,NULL,NULL,'percentheal 0,15;','','');


#  Ranger Special Alloy Trap
# =============================================================
REPLACE INTO `item_db2` VALUES (12341,'Special_Alloy_Trap_Box','Special Alloy Trap Box',2,NULL,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'getitem 7940,100;','','');

# =============================================================
REPLACE INTO `item_db2` VALUES (12380,'Former_Experimental_Flute','Former Experimental Flute',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12381,'Ancient_Language_Scroll_1','Ancient Language Scroll 1',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12382,'Ancient_Language_Scroll_2','Ancient Language Scroll 2',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12383,'Vulcan_Bullet_Magazine','Vulcan Bullet Magazine',2,10,NULL,500,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'getitem 6145,1000;','','');
REPLACE INTO `item_db2` VALUES (12384,'Magical_Ruby_1','Magical Ruby 1',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12385,'Magical_Ruby_2','Magical Ruby 2',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12386,'Magical_Ruby_3','Magical Ruby 3',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12387,'Magical_Ruby_4','Magical Ruby 4',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Mechanic Mado Gear Repair Items
# =============================================================
REPLACE INTO `item_db2` VALUES (12392,'Repair_A','Repair A',0,220,NULL,70,NULL,NULL,NULL,NULL,0x00000400,8,2,NULL,NULL,NULL,NULL,NULL,'itemheal rand(200,300),0;','','');
REPLACE INTO `item_db2` VALUES (12393,'Repair_B','Repair B',0,500,NULL,70,NULL,NULL,NULL,NULL,0x00000400,8,2,NULL,NULL,NULL,NULL,NULL,'itemheal rand(300,400),0;','','');
REPLACE INTO `item_db2` VALUES (12394,'Repair_C','Repair C',0,1100,NULL,70,NULL,NULL,NULL,NULL,0x00000400,8,2,NULL,NULL,NULL,NULL,NULL,'itemheal rand(400,500),0;','','');

# Genetic Created Boosting Items
# =============================================================
REPLACE INTO `item_db2` VALUES (12417,'Boost_500','Boost 500',2,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_BOOST500,500000,20;','','');
REPLACE INTO `item_db2` VALUES (12418,'Full_Swing_K','Full Swing K',2,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12419,'Manaplus','Manaplus',2,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12420,'Muramura_M','Muramura M',2,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12421,'Falmons_F','Falmons F',2,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12422,'HP_Increase_Potion_(Small)','HP Increase Potion (Small)',0,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_INCMHPRATE,500000,1; itemheal 840,0;','','');
REPLACE INTO `item_db2` VALUES (12423,'HP_Increase_Potion_(Medium)','HP Increase Potion (Medium)',0,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_INCMHPRATE,500000,2; itemheal 1040,0;','','');
REPLACE INTO `item_db2` VALUES (12424,'HP_Increase_Potion_(Large)','HP Increase Potion (Large)',0,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_INCMHPRATE,500000,5; itemheal 1240,0;','','');
REPLACE INTO `item_db2` VALUES (12425,'SP_Increase_Potion_(Small)','SP Increase Potion (Small)',0,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_INCMSPRATE,500000,2; itemheal 0,200;','','');
REPLACE INTO `item_db2` VALUES (12426,'SP_Increase_Potion_(Medium)','SP Increase Potion (Medium)',0,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_INCMSPRATE,500000,4; itemheal 0,300;','','');
REPLACE INTO `item_db2` VALUES (12427,'SP_Increase_Potion_(Large)','SP Increase Potion (Large)',0,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_INCMSPRATE,500000,8; itemheal 0,400;','','');
REPLACE INTO `item_db2` VALUES (12428,'Concentrated_White_Potion_Z','Concentrated White Potion Z',0,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12429,'Savage_Full_Roast','Savage Full Roast',2,NULL,NULL,50,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_SAVAGE_STEAK, 1800000, 20;','','');
REPLACE INTO `item_db2` VALUES (12430,'Cocktail_Warg_Blood','Cocktail Warg Blood',2,NULL,NULL,50,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_COCKTAIL_WARG_BLOOD, 1800000, 20;','','');
REPLACE INTO `item_db2` VALUES (12431,'Minor_Stew','Minor Stew',2,NULL,NULL,50,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_MINOR_BBQ, 1800000, 20;','','');
REPLACE INTO `item_db2` VALUES (12432,'Siroma_Iced_Tea','Siroma Iced Tea',2,NULL,NULL,50,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_SIROMA_ICE_TEA, 1800000, 20;','','');
REPLACE INTO `item_db2` VALUES (12433,'Drosera_Herb_Salad','Drosera Herb Salad',2,NULL,NULL,50,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_DROCERA_HERB_STEAMED, 1800000, 20;','','');
REPLACE INTO `item_db2` VALUES (12434,'Petite_Tail_Noodles','Petite Tail Noodles',2,NULL,NULL,50,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_PUTTI_TAILS_NOODLES, 1800000, 20;','','');
REPLACE INTO `item_db2` VALUES (12435,'Black_Mass','Black Mass',2,NULL,NULL,50,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'sc_start SC_STOMACHACHE, 60000,  rand(5,10);','','');
REPLACE INTO `item_db2` VALUES (12436,'Vitata_500','Vitata 500',0,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12437,'Concentrated_Ceromain_Soup','Concentrated Ceromain Soup',0,10,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,7,2,NULL,NULL,NULL,NULL,NULL,'','','');


# =============================================================
REPLACE INTO `item_db2` VALUES (12475,'Cure_Photo','Cure Photo',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Guillotine Cross Poisons
# =============================================================
REPLACE INTO `item_db2` VALUES (12717,'Poison_Paralysis','Paralyze',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(rand(10000) < 10) sc_start SC_PARALYSE,300000,0;','','');
REPLACE INTO `item_db2` VALUES (12718,'Poison_Leech','Leech End',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(rand(10000) < 10) sc_start SC_LEECHESEND,300000,0;','','');
REPLACE INTO `item_db2` VALUES (12719,'Poison_Oblivion','Oblivion Curse',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(rand(10000) < 10) sc_start SC_OBLIVIONCURSE,300000,0;','','');
REPLACE INTO `item_db2` VALUES (12720,'Poison_Disheart','Disheart',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(rand(10000) < 10) sc_start SC_DEATHHURT,300000,0;','','');
REPLACE INTO `item_db2` VALUES (12721,'Poison_Numb','Toxin',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(rand(10000) < 10) sc_start SC_TOXIN,300000,0;','','');
REPLACE INTO `item_db2` VALUES (12722,'Poison_Fever','Pyrexia',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(rand(10000) < 10) sc_start SC_PYREXIA,300000,0;','','');
REPLACE INTO `item_db2` VALUES (12723,'Poison_Laughing','Magic Mushroom',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(rand(10000) < 10) sc_start SC_MAGICMUSHROOM,300000,0;','','');
REPLACE INTO `item_db2` VALUES (12724,'Poison_Fatigue','Venom Bleed',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(rand(10000) < 10) sc_start SC_VENOMBLEED,15000,0;','','');

# Rune Knight's Rune Stones
# =============================================================
REPLACE INTO `item_db2` VALUES (12725,'Runstone_Nosiege','Nauthiz Rune',11,100,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(Class == Job_Rune_Knight || Class == Job_Rune_Knight_T) itemskill "RK_REFRESH",1;','','');
REPLACE INTO `item_db2` VALUES (12726,'Runstone_Rhydo','Raido Rune',11,100,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(Class == Job_Rune_Knight || Class == Job_Rune_Knight_T) itemskill "RK_CRUSHSTRIKE",1;','','');
REPLACE INTO `item_db2` VALUES (12727,'Runstone_Verkana','Berkana Rune',11,100,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(Class == Job_Rune_Knight || Class == Job_Rune_Knight_T) itemskill "RK_MILLENNIUMSHIELD",1;','','');
REPLACE INTO `item_db2` VALUES (12728,'Runstone_Isia','Isa Rune',11,100,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(Class == Job_Rune_Knight || Class == Job_Rune_Knight_T) itemskill "RK_VITALITYACTIVATION",1;','','');
REPLACE INTO `item_db2` VALUES (12729,'Runstone_Asir','Othila Rune',11,100,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(Class == Job_Rune_Knight || Class == Job_Rune_Knight_T) itemskill "RK_FIGHTINGSPIRIT",1;','','');
REPLACE INTO `item_db2` VALUES (12730,'Runstone_Urj','Uruz Rune',11,100,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(Class == Job_Rune_Knight || Class == Job_Rune_Knight_T) itemskill "RK_ABUNDANCE",1;','','');
REPLACE INTO `item_db2` VALUES (12731,'Runstone_Turisus','Thurisaz Rune',11,100,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(Class == Job_Rune_Knight || Class == Job_Rune_Knight_T) itemskill "RK_GIANTGROWTH",1;','','');
REPLACE INTO `item_db2` VALUES (12732,'Runstone_Pertz','Wyrd Rune',11,100,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(Class == Job_Rune_Knight || Class == Job_Rune_Knight_T) itemskill "RK_STORMBLAST",1;','','');

# Rune Knight Rune Ores
# =============================================================
REPLACE INTO `item_db2` VALUES (12733,'Runstone_Hagalas','Hagalaz Rune',11,100,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'if(Class == Job_Rune_Knight || Class == Job_Rune_Knight_T) itemskill "RK_STONEHARDSKIN",1;','','');
REPLACE INTO `item_db2` VALUES (12734,'Runstone_Quality','Luxurious Rune',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'makerune 1;','','');
REPLACE INTO `item_db2` VALUES (12735,'Runstone_Ancient','Ancient Rune',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'makerune 2;','','');
REPLACE INTO `item_db2` VALUES (12736,'Runstone_Mystic','Mystic Rune',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'makerune 3;','','');
REPLACE INTO `item_db2` VALUES (12737,'Runstone_Ordinary','General Rune',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'makerune 4;','','');
REPLACE INTO `item_db2` VALUES (12738,'Runstone_Rare','Rare Rune',0,2,NULL,100,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'makerune 5;','','');

# Genetic Created Bombs And Lumps
# =============================================================
REPLACE INTO `item_db2` VALUES (13260,'Apple_Bomb','Apple Bomb',17,10,NULL,100,1,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13261,'Coconut_Bomb','Coconut Bomb',17,10,NULL,1,100,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13262,'Melon_Bomb','Melon Bomb',17,10,NULL,100,1,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13263,'Pineapple_Bomb','Pineapple Bomb',17,10,NULL,1,100,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13264,'Banana_Bomb','Banana Bomb',17,10,NULL,1,100,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13265,'Black_Lump','Black Lump',17,10,NULL,50,100,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13266,'Hard_Black_Lump','Hard Black Lump',17,10,NULL,50,100,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13267,'Extremely_Hard_Black_Lump','Extremely Hard Black Lump',17,10,NULL,50,100,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13268,'Mysterious_Powder','Mysterious Powder',17,10,NULL,10,100,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'sc_start SC_MYSTERIOUS_POWDER,10000,2;','','');

# Generic Created Throwable Booster Items
# =============================================================
REPLACE INTO `item_db2` VALUES (13269,'Throwing_Boost_500','Throwing Boost 500',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13270,'Throwing_Full_Swing_K','Throwing Full Swing K',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13271,'Throwing_Manaplus','Throwing Manaplus',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13272,'Throwing_Cure_Free','Throwing Cure Free',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13273,'Throwing_Muramura_M','Throwing Muramura M',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13274,'Throwing_Falmons_F','Throwing Falmons F',11,17,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13275,'Throwing_Increase_HP_Potion_(Small)','Throwing Increase HP Potion (Small)',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'sc_start SC_INCMHPRATE,500000,1; itemheal 840,0;','','');
REPLACE INTO `item_db2` VALUES (13276,'Throwing_Increase_HP_Potion_(Medium)','Throwing Increase HP Potion (Medium)',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'sc_start SC_INCMHPRATE,500000,2; itemheal 1040,0;','','');
REPLACE INTO `item_db2` VALUES (13277,'Throwing_Increase_HP_Potion_(Large)','Throwing Increase HP Potion (Large)',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'sc_start SC_INCMHPRATE,500000,5; itemheal 1240,0;','','');
REPLACE INTO `item_db2` VALUES (13278,'Throwing_Increase_SP_Potion_(Small)','Throwing Increase SP Potion (Small)',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'sc_start SC_INCMSPRATE,500000,2; itemheal 0,200;','','');
REPLACE INTO `item_db2` VALUES (13279,'Throwing_Increase_SP_Potion_(Medium)','Throwing Increase SP Potion (Medium)',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'sc_start SC_INCMSPRATE,500000,4; itemheal 0,300;','','');
REPLACE INTO `item_db2` VALUES (13280,'Throwing_Increase_SP_Potion_(Large)','Throwing Increase SP Potion (Large)',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'sc_start SC_INCMSPRATE,500000,8; itemheal 0,400;','','');
REPLACE INTO `item_db2` VALUES (13281,'Throwing_Concentrated_White_Potion_Z','Throwing Concentrated White Potion Z',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13282,'Throwing_Vitata_500','Throwing Vitata 500',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13283,'Throwing_Ceromain_Soup','Throwing Ceromain Soup',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13284,'Throwing_Savage_Full_Roast','Throwing Savage Full Roast',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13285,'Throwing_Cocktail_Warg_Blood','Throwing Cocktail Warg Blood',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13286,'Throwing_Minor_Stew','Throwing Minor Stew',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13287,'Throwing_Siroma_Iced_Tea','Throwing Siroma Iced Tea',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13288,'Throwing_Drosera_Herb_Salad','Throwing Drosera Herb Salad',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13289,'Throwing_Petite_Tail_Soup','Throwing Petite Tail Soup',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');
REPLACE INTO `item_db2` VALUES (13290,'Throwing_Black_Mass','Throwing Black Mass',17,10,NULL,100,0,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,9,'','','');

# Unknown Item. Rune Knight Armor???
# =============================================================
REPLACE INTO `item_db2` VALUES (15002,'Rune_Plate','Rune Plate',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');

# Mechanic Mado Gear Cannonball Ammo
# =============================================================
REPLACE INTO `item_db2` VALUES (18000,'Cannon_Ball','Cannon Ball',19,100,NULL,19,100,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,8,'','','');
REPLACE INTO `item_db2` VALUES (18001,'Holy_Cannon_Ball','Holy Cannon Ball',19,200,NULL,10,120,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,8,'bonus bAtkEle,Ele_Holy;','','');
REPLACE INTO `item_db2` VALUES (18002,'Dark_Cannon_Ball','Dark Cannon Ball',19,200,NULL,10,120,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,8,'bonus bAtkEle,Ele_Dark;','','');
REPLACE INTO `item_db2` VALUES (18003,'Soul_Cannon_Ball','Soul Cannon Ball',19,200,NULL,10,120,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,8,'bonus bAtkEle,Ele_Ghost;','','');
REPLACE INTO `item_db2` VALUES (18004,'Iron_Cannon_Ball','Iron Cannon Ball',19,500,NULL,10,250,NULL,NULL,NULL,0xFFFFFFFF,8,2,32768,NULL,1,NULL,8,'bonus bAtkEle,Ele_Neutral;','','');

# New Weapons In Work And Testing
# =============================================================
REPLACE INTO `item_db2` VALUES (1196,'Chrome_Metal_Two-Hand_Sword','Chrome Metal Two-Hand Sword',4,10,NULL,400,280,NULL,1,0,0x00004080,8,2,34,3,110,1,3,'','','');
REPLACE INTO `item_db2` VALUES (1433,'Imperial_Spear','Imperial Spear',4,10,NULL,1800,180,NULL,3,1,0x00004000,8,2,2,3,102,1,2,'','','');
REPLACE INTO `item_db2` VALUES (1654,'Mental_Stick','Mental Stick',4,10,NULL,500,40,NULL,1,1,0x00010000,8,2,2,3,102,1,10,'','','');
REPLACE INTO `item_db2` VALUES (1830,'Great_King\'s_Dance','Great King\'s Dance',4,10,NULL,500,142,NULL,1,1,0x00008000,8,2,2,3,102,1,12,'','','');
REPLACE INTO `item_db2` VALUES (1930,'Green_Whistle','Green Whistle',4,10,NULL,800,170,NULL,1,1,0x00080000,8,1,2,3,102,1,13,'','','');
REPLACE INTO `item_db2` VALUES (1984,'Stem_Whip','Stem Whip',4,10,NULL,800,170,NULL,2,1,0x00080000,8,0,2,3,102,1,14,'','','');
REPLACE INTO `item_db2` VALUES (1985,'Rose_Vine','Rose Vine',4,10,NULL,1000,100,NULL,2,0,0x00080000,8,0,2,4,110,1,14,'','','');
REPLACE INTO `item_db2` VALUES (2153,'Imperial_Guard','Imperial Guard',5,10,NULL,2500,NULL,12,NULL,1,0x00004000,8,2,32,NULL,102,1,3,'','','');
REPLACE INTO `item_db2` VALUES (13061,'Black_Wing','Black Wing',4,10,NULL,600,142,NULL,1,1,0x00020000,8,2,2,3,102,1,1,'','','');
REPLACE INTO `item_db2` VALUES (13062,'Ancient_Dagger','Ancient Dagger',4,10,NULL,600,107,NULL,1,0,0x000F5E80,8,2,2,4,120,1,1,'','','');
REPLACE INTO `item_db2` VALUES (13431,'Chrome_Metal_Sword','Chrome Metal Sword',4,10,NULL,2200,180,NULL,1,0,0x00004080,8,2,2,3,110,1,2,'','','');
REPLACE INTO `item_db2` VALUES (16010,'Red_Ether_Bag','Red Ether Bag',4,10,NULL,1000,15,NULL,1,1,0x00040000,8,2,2,3,102,1,8,'','','');
REPLACE INTO `item_db2` VALUES (18103,'Mystic_Bow','Mystic Bow',4,10,NULL,1700,75,NULL,5,0,0x00080800,8,2,34,3,105,1,11,'','','');

# 3rd Job Exclusive Headgears V2 (Ballanced A Week After Released)
# =============================================================
REPLACE INTO `item_db2` VALUES (5746,'Rune_Circlet','Rune Circlet',5,20,NULL,100,NULL,4,NULL,1,0x00000080,8,2,256,NULL,100,1,623,'bonus bStr,1; bonus bInt,1; bonus bMdef,5; if(readparam(bStr)>=120){ bonus bAtk,10; bonus bMatkRate,5; };','','');
REPLACE INTO `item_db2` VALUES (5747,'Mitra','Mitra',5,20,NULL,100,NULL,4,NULL,1,0x00000100,8,2,256,NULL,100,1,624,'bonus bVit,1; bonus bInt,1; bonus bMdef,5; bonus bHealPower,5; if(readparam(bInt)>=120){ bonus bMatkRate,10; };','','');
REPLACE INTO `item_db2` VALUES (5748,'Sniper_Goggles','Sniper Goggles',5,20,NULL,100,NULL,4,NULL,1,0x00000800,8,2,768,NULL,100,1,625,'bonus bAgi,1; bonus bDex,1; bonus bMdef,5; if(readparam(bAgi)>=120){ bonus bAtkRate,4; bonus bAspd,1; };','','');
REPLACE INTO `item_db2` VALUES (5749,'Driver_Band_M','Driver Band',5,20,NULL,100,NULL,4,NULL,1,0x00000400,8,1,256,NULL,100,1,626,'bonus bStr,1; bonus bDex,1; bonus bMdef,5; if(readparam(bStr)>=120){ bonus bAtk,10; bonus bCritical,3; };','','');
REPLACE INTO `item_db2` VALUES (5750,'Shadow_Crown','Shadow Crown',5,20,NULL,100,NULL,4,NULL,1,0x00020000,8,2,256,NULL,100,1,627,'bonus bAgi,1; bonus bInt,1; bonus bMdef,1; if(readparam(bAgi)>=120){ bonus bAtk,10; bonus bFlee,3; };','','');
REPLACE INTO `item_db2` VALUES (5751,'Minstrel_Song_Hat','Minstrel Song Hat',5,20,NULL,100,NULL,4,NULL,1,0x00080000,8,1,256,NULL,100,1,628,'bonus bInt,1; bonus bLuk,1; bonus bMdef,5; bonus bAtkRate,5; bonus bUseSPrate,-10;','','');
REPLACE INTO `item_db2` VALUES (5752,'Midas_Whispers','Midas Whispers',5,20,NULL,100,NULL,4,NULL,1,0x00040000,8,2,256,NULL,100,1,629,'bonus bStr,1; bonus bDex,1; bonus bMdef,5; if(readparam(bStr)>=120){ bonus bAtk,5; bonus bAspd,1; };','','');
REPLACE INTO `item_db2` VALUES (5753,'Magic_Stone_Hat','Magic Stone Hat',5,20,NULL,100,NULL,4,NULL,1,0x00000200,8,2,256,NULL,100,1,630,'bonus bInt,1; bonus bDex,1; bonus bMdef,5; if(readparam(bDex)>=120){ bonus bMatkRate,10; bonus bCastrate,-2; };','','');
REPLACE INTO `item_db2` VALUES (5754,'Burning_Spirit','Burning Spirit',5,20,NULL,100,NULL,4,NULL,1,0x00008000,8,2,256,NULL,100,1,631,'bonus bStr,1; bonus bVit,1; bonus bMdef,5; if(readparam(bStr)>=120){ bonus bAtk,10; bonus bHit,3; };','','');
REPLACE INTO `item_db2` VALUES (5755,'Silent_Enforcer','Silent Enforcer',5,20,NULL,100,NULL,2,NULL,0,0x00001000,8,2,513,NULL,100,1,632,'bonus bAgi,1; bonus bMdef,5; if(readparam(bAgi)>=120){ bonus bAtk,10; bonus bFlee2,5; };','','');
REPLACE INTO `item_db2` VALUES (5756,'Wispers_of_Wind','Wispers of Wind',5,20,NULL,100,NULL,4,NULL,1,0x00010000,8,2,256,NULL,100,1,633,'bonus bInt,1; bonus bDex,1; bonus bMdef,5; if(readparam(bInt)>=120){ bonus bMatkRate,10; bonus bFlee,3; };','','');
REPLACE INTO `item_db2` VALUES (5757,'Reissue_Schmitz_Helm','Reissue Schmitz Helm',5,20,NULL,100,NULL,6,NULL,1,0x00004000,8,2,768,NULL,100,1,634,'bonus bVit,2; bonus bInt,1; bonus bMdef,5; if(readparam(bInt)>=120){ bonus bMatkRate,10; bonus bDef,5; };','','');
REPLACE INTO `item_db2` VALUES (5758,'Resting_Swan','Resting Swan',5,20,NULL,100,NULL,4,NULL,1,0x00080000,8,0,256,NULL,100,1,635,'bonus bInt,1; bonus bLuk,1; bonus bMdef,5; bonus bAtkRate,5; bonus bUseSPrate,-10;','','');
REPLACE INTO `item_db2` VALUES (5760,'Driver_Band_F','Driver Band',5,20,NULL,100,NULL,4,NULL,1,0x00000400,8,0,256,NULL,100,1,637,'bonus bStr,1; bonus bDex,1; bonus bMdef,5; if(readparam(bStr)>=120){ bonus bAtk,10; bonus bCritical,3; };','','');

# New Test Items
# =============================================================
REPLACE INTO `item_db2` VALUES (2463,'Wild_Boots','Wild Boots',5,10,NULL,0,NULL,12,NULL,0,0xFFFFFFFF,7,2,64,NULL,75,1,0,'bonus bUnbreakableShoes,0;','','');
REPLACE INTO `item_db2` VALUES (2465,'Dance_Shoes','Dance Shoes',5,10,NULL,300,NULL,15,NULL,0,0x00080000,8,0,64,NULL,105,1,0,'bonus bAgi,1; bonus bAspdRate,2;','','');
REPLACE INTO `item_db2` VALUES (2564,'Wild_Tail','Wild Tail',5,10,NULL,0,NULL,16,NULL,0,0xFFFFFFFF,7,2,4,NULL,75,1,0,'bonus bUnbreakableGarment,0;','','');
REPLACE INTO `item_db2` VALUES (2843,'Gold_Drops','Gold Drops',5,10,NULL,100,NULL,0,NULL,0,0xFFFFFFFF,7,2,136,NULL,75,0,0,'bonus bMaxSP,50;','','');
REPLACE INTO `item_db2` VALUES (2844,'Light_of_El_Dicastes','Light of El Dicastes',5,20,NULL,100,NULL,0,NULL,0,0xFFFFFFFF,7,2,136,NULL,80,0,0,'skill "RETURN_TO_ELDICASTES",1;','','');
REPLACE INTO `item_db2` VALUES (2853,'Kinetic_Of_Telemarketing','Kinetic Of Telemarketing',5,10,NULL,20,NULL,2,NULL,0,0xFFFFFFFE,8,2,136,NULL,110,0,0,'bonus bInt,3; bonus bMdef,1; bonus bMaxSP,30;','','');
REPLACE INTO `item_db2` VALUES (2854,'Glove_Of_Alchemy','Glove Of Alchemy',5,10,NULL,100,NULL,1,NULL,0,0xFFFFFFFE,8,2,136,NULL,125,0,0,'bonus bInt,1; bonus bMdef,2;','','');
REPLACE INTO `item_db2` VALUES (4760,'MagicalAttack1%','MATK+1%',6,20,NULL,100,NULL,NULL,NULL,NULL,NULL,NULL,NULL,16,NULL,NULL,NULL,NULL,'bonus bMatkRate,1; bonus bFixCastRate,-1;','','');
REPLACE INTO `item_db2` VALUES (4761,'MagicalAttack2%','MATK+2%',6,20,NULL,100,NULL,NULL,NULL,NULL,NULL,NULL,NULL,16,NULL,NULL,NULL,NULL,'bonus bMatkRate,2; bonus bFixCastRate,-1;','','');
REPLACE INTO `item_db2` VALUES (4762,'Flee6','FLEE+6',6,20,NULL,100,NULL,NULL,NULL,NULL,NULL,NULL,NULL,16,NULL,NULL,NULL,NULL,'bonus bFlee,6;','','');
REPLACE INTO `item_db2` VALUES (4763,'Flee12','FLEE+12',6,20,NULL,100,NULL,NULL,NULL,NULL,NULL,NULL,NULL,16,NULL,NULL,NULL,NULL,'bonus bFlee,12;','','');
REPLACE INTO `item_db2` VALUES (4764,'Critical5','CRI+5',6,20,NULL,100,NULL,NULL,NULL,NULL,NULL,NULL,NULL,16,NULL,NULL,NULL,NULL,'bonus bCritical,5;','','');
REPLACE INTO `item_db2` VALUES (4765,'Critical7','CRI+7',6,20,NULL,100,NULL,NULL,NULL,NULL,NULL,NULL,NULL,16,NULL,NULL,NULL,NULL,'bonus bCritical,7;','','');
REPLACE INTO `item_db2` VALUES (4766,'Attack2%','ATK+2%',6,20,NULL,100,NULL,NULL,NULL,NULL,NULL,NULL,NULL,16,NULL,NULL,NULL,NULL,'bonus bAtkRate,2;','','');
REPLACE INTO `item_db2` VALUES (4767,'Attack3%','ATK+3%',6,20,NULL,100,NULL,NULL,NULL,NULL,NULL,NULL,NULL,16,NULL,NULL,NULL,NULL,'bonus bAtkRate,3;','','');
REPLACE INTO `item_db2` VALUES (5742,'Rudolph_Santa_Hat','Rudolph_Santa_Hat',5,20,NULL,200,NULL,5,NULL,0,0xFFFFFFFE,7,2,256,NULL,1,1,98,'bonus bMaxHP,300;','','');
REPLACE INTO `item_db2` VALUES (6305,'Frozen_Skin_Pieces','Frozen_Skin_Pieces',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6306,'Blood_Almost_Hardened','Blood Almost Hardened',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6307,'Stone\'s_Award-winning_Horsepower','Stone\'s Award-winning Horsepower',3,10,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (6377,'Buy_Stall_Permit','Buy Stall Permit',3,200,NULL,10,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'','','');
REPLACE INTO `item_db2` VALUES (12548,'Buy_Shabby_Stall_Permit','Buy Shabby Stall Permit',11,500,NULL,10,NULL,NULL,NULL,NULL,0xFFFFFFFF,8,2,NULL,NULL,NULL,NULL,NULL,'itemskill "ALL_BUYING_STORE",1;','','');
