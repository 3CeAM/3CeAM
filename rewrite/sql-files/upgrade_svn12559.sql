--
-- Table structure for table `quest`
--

DROP TABLE IF EXISTS `quest`;
CREATE TABLE `quest` (
  `char_id` int(11) unsigned NOT NULL default '0',
  `quest_id` int(10) unsigned NOT NULL,
  `state` enum('1','0') NOT NULL default '0',
  PRIMARY KEY  USING BTREE (`char_id`,`quest_id`)
) ENGINE=MyISAM;

--
-- Table structure for table `quest_mob`
--

DROP TABLE IF EXISTS `quest_objective`;
CREATE TABLE `quest_objective` (
  `quest_id` int(11) unsigned NOT NULL,
  `count` mediumint(8) unsigned NOT NULL default '0',
  `name` varchar(255) NOT NULL default '',
  `num` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY  USING BTREE (`quest_id`,`num`)
) ENGINE=MyISAM;
