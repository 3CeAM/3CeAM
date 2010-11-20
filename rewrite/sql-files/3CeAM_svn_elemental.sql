--
-- Table structure for table `elemental`
--

CREATE TABLE `elemental` (
  `ele_id` int(11) unsigned NOT NULL auto_increment,
  `char_id` int(11) NOT NULL,
  `class` mediumint(9) unsigned NOT NULL default '0',
  `mode` int(11) unsigned NOT NULL default '1',
  `hp` int(12) NOT NULL default '1',
  `sp` int(12) NOT NULL default '1',
  `life_time` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ele_id`)
) ENGINE=MyISAM;


--
-- Add `elemental_id` clumn in `char` table.
--
ALTER TABLE `char` ADD COLUMN `elemental_id` INTEGER UNSIGNED NOT NULL DEFAULT '0' AFTER `homun_id`;