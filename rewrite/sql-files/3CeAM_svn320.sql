--
-- Table structure for table `skillcooldown`
--

CREATE TABLE IF NOT EXISTS `skillcooldown` (
  `account_id` int(11) unsigned NOT NULL,
  `char_id` int(11) unsigned NOT NULL,
  `skill` smallint(11) unsigned NOT NULL default '0',
  `tick` int(11) NOT NULL,
  KEY (`account_id`),
  KEY (`char_id`)
) ENGINE=MyISAM;
