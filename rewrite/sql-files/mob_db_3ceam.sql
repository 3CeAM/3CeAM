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
#  Mechanic Fixed Autonomous Weapon Platforms
REPLACE INTO `mob_db_3ceam` VALUES (2042,'SILVERSNIPER','Silver Sniper','Silver Sniper',100,4500,0,0,0,9,0,390,15,8,10,60,10,10,120,10,10,12,1,0,20,0x3394,2000,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2043,'MAGICDECOY_FIRE','Magic Decoy Fire','Magic Decoy Fire',100,2500,0,0,0,7,0,195,3,33,10,10,10,100,60,10,10,12,1,0,23,0x3394,2000,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2044,'MAGICDECOY_WATER','Magic Decoy Water','Magic Decoy Water',100,2500,0,0,0,7,0,195,3,33,10,10,10,100,60,10,10,12,1,0,21,0x3394,2000,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2045,'MAGICDECOY_EARTH','Magic Decoy Earth','Magic Decoy Earth',100,2500,0,0,0,7,0,195,3,33,10,10,10,100,60,10,10,12,1,0,22,0x3394,2000,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2046,'MAGICDECOY_WIND','Magic Decoy Wind','Magic Decoy Wind',100,2500,0,0,0,7,0,195,3,33,10,10,10,100,60,10,10,12,1,0,24,0x3394,2000,504,1020,360,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
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
REPLACE INTO `mob_db_3ceam` VALUES (2158,'S_HORNET','Hornet','Hornet',95,5000,0,0,0,1,113,381,1,0,62,54,54,65,76,55,10,12,0,4,24,0x1189,150,1292,792,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2159,'S_GIANT_HORNET','Giant Hornet','Giant Hornet',110,11960,0,0,0,1,123,456,15,27,117,65,67,72,77,34,10,12,0,4,24,0x3795,155,1292,792,340,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
REPLACE INTO `mob_db_3ceam` VALUES (2160,'S_LUCIOLA_VESPA','Luciola Vespa','Luciola Vespa',130,16466,0,0,0,1,154,677,11,31,88,89,55,32,119,59,10,12,1,4,24,0x7795,110,1000,864,432,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
#  Kagerou/Oboro Zanzou - Settings Are Not Official.
REPLACE INTO `mob_db_3ceam` VALUES (2308,'KO_KAGE','Shadow Clone','Shadow Clone',100,18000,0,0,0,1,0,0,0,0,1,1,1,1,1,1,10,12,1,0,27,0x0,150,1003,1152,336,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

