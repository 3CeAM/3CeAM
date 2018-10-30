#
# Table structure for table `mob_db_3ceam`
#

DROP TABLE IF EXISTS `mob_db_3ceam`;
CREATE TABLE `mob_db_3ceam` (
  `ID` mediumint(9) unsigned NOT NULL default '0',
  `Sprite` text NOT NULL,
  `kName` text NOT NULL,
  `iName` text NOT NULL,
  `LV` tinyint(6) unsigned NOT NULL default '0',
  `HP` int(9) unsigned NOT NULL default '0',
  `SP` mediumint(9) unsigned NOT NULL default '0',
  `EXP` mediumint(9) unsigned NOT NULL default '0',
  `JEXP` mediumint(9) unsigned NOT NULL default '0',
  `Range1` tinyint(4) unsigned NOT NULL default '0',
  `ATK1` smallint(6) unsigned NOT NULL default '0',
  `ATK2` smallint(6) unsigned NOT NULL default '0',
  `DEF` smallint(6) unsigned NOT NULL default '0',
  `MDEF` smallint(6) unsigned NOT NULL default '0',
  `STR` smallint(6) unsigned NOT NULL default '0',
  `AGI` smallint(6) unsigned NOT NULL default '0',
  `VIT` smallint(6) unsigned NOT NULL default '0',
  `INT` smallint(6) unsigned NOT NULL default '0',
  `DEX` smallint(6) unsigned NOT NULL default '0',
  `LUK` smallint(6) unsigned NOT NULL default '0',
  `Range2` tinyint(4) unsigned NOT NULL default '0',
  `Range3` tinyint(4) unsigned NOT NULL default '0',
  `Scale` tinyint(4) unsigned NOT NULL default '0',
  `Race` tinyint(4) unsigned NOT NULL default '0',
  `Element` tinyint(4) unsigned NOT NULL default '0',
  `Mode` smallint(6) unsigned NOT NULL default '0',
  `Speed` smallint(6) unsigned NOT NULL default '0',
  `aDelay` smallint(6) unsigned NOT NULL default '0',
  `aMotion` smallint(6) unsigned NOT NULL default '0',
  `dMotion` smallint(6) unsigned NOT NULL default '0',
  `MEXP` mediumint(9) unsigned NOT NULL default '0',
  `ExpPer` smallint(9) unsigned NOT NULL default '0',
  `MVP1id` smallint(9) unsigned NOT NULL default '0',
  `MVP1per` smallint(9) unsigned NOT NULL default '0',
  `MVP2id` smallint(9) unsigned NOT NULL default '0',
  `MVP2per` smallint(9) unsigned NOT NULL default '0',
  `MVP3id` smallint(9) unsigned NOT NULL default '0',
  `MVP3per` smallint(9) unsigned NOT NULL default '0',
  `Drop1id` smallint(9) unsigned NOT NULL default '0',
  `Drop1per` smallint(9) unsigned NOT NULL default '0',
  `Drop2id` smallint(9) unsigned NOT NULL default '0',
  `Drop2per` smallint(9) unsigned NOT NULL default '0',
  `Drop3id` smallint(9) unsigned NOT NULL default '0',
  `Drop3per` smallint(9) unsigned NOT NULL default '0',
  `Drop4id` smallint(9) unsigned NOT NULL default '0',
  `Drop4per` smallint(9) unsigned NOT NULL default '0',
  `Drop5id` smallint(9) unsigned NOT NULL default '0',
  `Drop5per` smallint(9) unsigned NOT NULL default '0',
  `Drop6id` smallint(9) unsigned NOT NULL default '0',
  `Drop6per` smallint(9) unsigned NOT NULL default '0',
  `Drop7id` smallint(9) unsigned NOT NULL default '0',
  `Drop7per` smallint(9) unsigned NOT NULL default '0',
  `Drop8id` smallint(9) unsigned NOT NULL default '0',
  `Drop8per` smallint(9) unsigned NOT NULL default '0',
  `Drop9id` smallint(9) unsigned NOT NULL default '0',
  `Drop9per` smallint(9) unsigned NOT NULL default '0',
  `DropCardid` smallint(9) unsigned NOT NULL default '0',
  `DropCardper` smallint(9) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM;

#  Monsters Additional Database
# 
#  Structure of Database :
# REPLACE INTO `mob_db_3ceam` VALUES ( ID,'Sprite_Name','kROName','iROName',LV,HP,SP,EXP,JEXP,Range1,ATK1,ATK2,DEF,MDEF,STR,AGI,VIT,INT,DEX,LUK,Range2,Range3,Scale,Race,Element,Mode,Speed,aDelay,aMotion,dMotion,MEXP,ExpPer,MVP1id,MVP1per,MVP2id,MVP2per,MVP3id,MVP3per,Drop1id,Drop1per,Drop2id,Drop2per,Drop3id,Drop3per,Drop4id,Drop4per,Drop5id,Drop5per,Drop6id,Drop6per,Drop7id,Drop7per,Drop8id,Drop8per,Drop9id,Drop9per,DropCardid,DropCardper);
#  Mechanic Fixed Autonomous Weapon Platforms - Note: DEF/MDEF settings is correct, but I need to figure out if DEF was changed in 2011 balance and a proper convert to pre-renewal DEF.
#  HP for all FAW's will use a temporarly fixed amount that a level 150 Mechanic with 120 stats and using max level FAW will give to the summoned FAW's.
REPLACE INTO `mob_db_3ceam` VALUES (2042,'SILVERSNIPER','Silver Sniper','Silver Sniper',100,17200,0,0,0,9,1000,1000,80,10,10,60,10,10,100,10,10,12,1,0,20,0x3394,2000,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2043,'MAGICDECOY_FIRE','Magic Decoy Fire','Magic Decoy Fire',100,13700,0,0,0,7,150,150,16,60,10,10,10,100,50,10,10,12,1,0,23,0x3394,2000,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2044,'MAGICDECOY_WATER','Magic Decoy Water','Magic Decoy Water',100,13700,0,0,0,7,150,150,16,60,10,10,10,100,50,10,10,12,1,0,21,0x3394,2000,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2045,'MAGICDECOY_EARTH','Magic Decoy Earth','Magic Decoy Earth',100,13700,0,0,0,7,150,150,16,60,10,10,10,100,50,10,10,12,1,0,22,0x3394,2000,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2046,'MAGICDECOY_WIND','Magic Decoy Wind','Magic Decoy Wind',100,13700,0,0,0,7,150,150,16,60,10,10,10,100,50,10,10,12,1,0,24,0x3394,2000,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
#  Sorcerer Elemental Spirits
REPLACE INTO `mob_db_3ceam` VALUES (2114,'EL_AGNI_S','Agni','Agni',100,5000,0,0,0,1,250,250,10,10,25,25,25,25,25,25,10,12,0,0,83,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2115,'EL_AGNI_M','Agni','Agni',100,7500,0,0,0,1,500,500,25,25,50,50,50,50,50,50,10,12,1,0,83,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2116,'EL_AGNI_L','Agni','Agni',100,10000,0,0,0,1,1000,1000,50,50,100,100,100,100,100,100,10,12,2,0,83,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2117,'EL_AQUA_S','Aqua','Aqua',100,5000,0,0,0,1,250,250,10,10,25,25,25,25,25,25,10,12,0,0,81,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2118,'EL_AQUA_M','Aqua','Aqua',100,7500,0,0,0,1,500,500,25,25,50,50,50,50,50,50,10,12,1,0,81,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2119,'EL_AQUA_L','Aqua','Aqua',100,10000,0,0,0,1,1000,1000,50,50,100,100,100,100,100,100,10,12,2,0,81,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2120,'EL_VENTUS_S','Ventus','Ventus',100,5000,0,0,0,4,250,250,10,10,25,25,25,25,25,25,10,12,0,0,84,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2121,'EL_VENTUS_M','Ventus','Ventus',100,7500,0,0,0,4,500,500,25,25,50,50,50,50,50,50,10,12,1,0,84,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2122,'EL_VENTUS_L','Ventus','Ventus',100,10000,0,0,0,4,1000,1000,50,50,100,100,100,100,100,100,10,12,2,0,84,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2123,'EL_TERA_S','Tera','Tera',100,5000,0,0,0,1,250,250,10,10,25,25,25,25,25,25,10,12,0,0,82,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2124,'EL_TERA_M','Tera','Tera',100,7500,0,0,0,1,500,500,25,25,50,50,50,50,50,50,10,12,1,0,82,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2125,'EL_TERA_L','Tera','Tera',100,10000,0,0,0,1,1000,1000,50,50,100,100,100,100,100,100,10,12,2,0,82,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2114,'EL_AGNI_S','Agni','Agni	',100,5000 ,0,0,0,1,100,100,0,0,10,10,10,10,10,10,10,12,0,0,83,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2115,'EL_AGNI_M','Agni','Agni	',100,7500 ,0,0,0,1,100,100,0,0,10,10,10,10,10,10,10,12,1,0,83,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2116,'EL_AGNI_L','Agni','Agni	',100,10000,0,0,0,1,100,100,0,0,10,10,10,10,10,10,10,12,2,0,83,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2117,'EL_AQUA_S','Aqua','Aqua	',100,5000 ,0,0,0,1,100,100,0,0,10,10,10,10,10,10,10,12,0,0,81,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2118,'EL_AQUA_M','Aqua','Aqua	',100,7500 ,0,0,0,1,100,100,0,0,10,10,10,10,10,10,10,12,1,0,81,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2119,'EL_AQUA_L','Aqua','Aqua	',100,10000,0,0,0,1,100,100,0,0,10,10,10,10,10,10,10,12,2,0,81,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2120,'EL_VENTUS_S','Ventus','Ventus',100,5000 ,0,0,0,4,100,100,0,0,10,10,10,10,10,10,10,12,0,0,84,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2121,'EL_VENTUS_M','Ventus','Ventus',100,7500 ,0,0,0,4,100,100,0,0,10,10,10,10,10,10,10,12,1,0,84,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2122,'EL_VENTUS_L','Ventus','Ventus',100,10000,0,0,0,4,100,100,0,0,10,10,10,10,10,10,10,12,2,0,84,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2123,'EL_TERA_S','Tera','Tera	',100,5000 ,0,0,0,1,100,100,0,0,10,10,10,10,10,10,10,12,0,0,82,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2124,'EL_TERA_M','Tera','Tera	',100,7500 ,0,0,0,1,100,100,0,0,10,10,10,10,10,10,10,12,1,0,82,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
# REPLACE INTO `mob_db_3ceam` VALUES (2125,'EL_TERA_L','Tera','Tera	',100,10000,0,0,0,1,100,100,0,0,10,10,10,10,10,10,10,12,2,0,82,0x83,200,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
#  Summon Legion Insects
REPLACE INTO `mob_db_3ceam` VALUES (2158,'S_HORNET','Hornet','Hornet',8,169,0,19,15,1,22,27,5,5,6,20,8,10,17,5,10,12,0,4,24,0x1189,150,1292,792,216,0,0,0,0,0,0,0,0,992,80,939,9000,909,3500,1208,15,511,350,518,150,0,0,0,0,0,0,4019,1);
REPLACE INTO `mob_db_3ceam` VALUES (2159,'S_GIANT_HORNET','Giant Hornet','Giant Hornet',56,13105,0,5785,2006,1,650,852,38,43,35,38,32,10,71,64,10,12,0,4,24,0x3795,155,1292,792,340,0,0,0,0,0,0,0,0,526,550,518,1200,522,12,610,15,1608,3,722,20,1736,15,0,0,0,0,4271,1);
REPLACE INTO `mob_db_3ceam` VALUES (2160,'S_LUCIOLA_VESPA','Luciola Vespa','Luciola Vespa',104,25103,0,13040,11410,1,9000,9900,29,5,104,56,30,20,116,4,10,12,1,4,24,0x7795,100,672,500,192,0,0,0,0,0,0,0,0,939,9000,955,9000,943,3000,518,300,526,200,992,160,2744,2,0,0,0,0,0,0);
#  Kagerou/Oboro Zanzou - Temporarly uses Shinobi stats and settings since it uses the Shinobi sprite.
REPLACE INTO `mob_db_3ceam` VALUES (2308,'KO_ZANZOU','Shadow Clone','Shadow Clone',100,12700,0,4970,3010,2,460,1410,34,21,85,85,25,25,100,100,10,12,1,7,67,0x0,150,1003,1152,336,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
#  Genetic Mutated Homunculus
# REPLACE INTO `mob_db_3ceam` VALUES (6048,'MER_EIRA','Eira','Eira',1,50,0,2,1,1,7,10,0,5,1,1,1,0,6,30,10,12,1,3,21,0x83,400,1872,672,480,0,0,0,0,0,0,0,0,909,7000,1202,100,938,400,512,1000,713,1500,512,150,619,20,0,0,0,0,4001,1);
# REPLACE INTO `mob_db_3ceam` VALUES (6049,'MER_BAYERI','Bayeri','Bayeri',1,50,0,2,1,1,7,10,0,5,1,1,1,0,6,30,10,12,1,3,21,0x83,400,1872,672,480,0,0,0,0,0,0,0,0,909,7000,1202,100,938,400,512,1000,713,1500,512,150,619,20,0,0,0,0,4001,1);
# REPLACE INTO `mob_db_3ceam` VALUES (6050,'MER_SERA','Sera','Sera',1,50,0,2,1,1,7,10,0,5,1,1,1,0,6,30,10,12,1,3,21,0x83,400,1872,672,480,0,0,0,0,0,0,0,0,909,7000,1202,100,938,400,512,1000,713,1500,512,150,619,20,0,0,0,0,4001,1);
# REPLACE INTO `mob_db_3ceam` VALUES (6051,'MER_DIETER','Dieter','Dieter',1,50,0,2,1,1,7,10,0,5,1,1,1,0,6,30,10,12,1,3,21,0x83,400,1872,672,480,0,0,0,0,0,0,0,0,909,7000,1202,100,938,400,512,1000,713,1500,512,150,619,20,0,0,0,0,4001,1);
# REPLACE INTO `mob_db_3ceam` VALUES (6052,'MER_ELEANOR','Eleanor','Eleanor',1,50,0,2,1,1,7,10,0,5,1,1,1,0,6,30,10,12,1,3,21,0x83,400,1872,672,480,0,0,0,0,0,0,0,0,909,7000,1202,100,938,400,512,1000,713,1500,512,150,619,20,0,0,0,0,4001,1);

