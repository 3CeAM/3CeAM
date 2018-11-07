#!/bin/bash

# This file is part of Hercules.
# http://herc.ws - http://github.com/HerculesWS/Hercules
#
# Copyright (C) 2014-2015  Hercules Dev Team
#
# Hercules is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Base Author: Haru @ http://herc.ws

MODE="$1"
shift

function foo {
	for i in "$@"; do
		echo "> $i"
	done
}

function usage {
	echo "usage:"
	echo "    $0 createdb <dbname> [dbuser] [dbpassword]"
	echo "    $0 importdb <dbname> [dbuser] [dbpassword]"
	echo "    $0 build [configure args]"
	echo "    $0 test <dbname> [dbuser] [dbpassword]"
	exit 1
}

function aborterror {
	echo $@
	exit 1
}

case "$MODE" in
	createdb|importdb|test)
		DBNAME="$1"
		DBUSER="$2"
		DBPASS="$3"
		if [ -z "$DBNAME" ]; then
			usage
		fi
		if [ "$MODE" != "test" ]; then
			if [ -n "$DBUSER" ]; then
				DBUSER="-u $DBUSER"
			fi
			if [ -n "$DBPASS" ]; then
				DBPASS="-p$DBPASS"
			fi
		fi
		;;
esac

case "$MODE" in
	createdb)
		echo "Creating database $DBNAME..."
		mysql $DBUSER $DBPASS -e "create database $DBNAME;" || aborterror "Unable to create database."
		;;
	importdb)
		echo "Importing tables into $DBNAME..."
		mysql $DBUSER $DBPASS $DBNAME < rewrite/sql-files/main.sql || aborterror "Unable to import main database."
		mysql $DBUSER $DBPASS $DBNAME < rewrite/sql-files/logs.sql || aborterror "Unable to import logs database."
		;;
	build)
		cd rewrite
		./configure $@ || aborterror "Configure error, aborting build."
		make sql -j3 || aborterror "Build failed."
		;;
	test)
		cat >> rewrite/conf/import/login_conf.txt << EOF
ipban.sql.db_username: $DBUSER
ipban.sql.db_password: $DBPASS
ipban.sql.db_database: $DBNAME
account.sql.db_username: $DBUSER
account.sql.db_password: $DBPASS
account.sql.db_database: $DBNAME
account.sql.db_hostname: localhost
EOF
		[ $? -eq 0 ] || aborterror "Unable to import configuration, aborting tests."
		cat >> rewrite/conf/import/inter_conf.txt << EOF
sql.db_username: $DBUSER
sql.db_password: $DBPASS
sql.db_database: $DBNAME
sql.db_hostname: localhost
char_server_id: $DBUSER
char_server_pw: $DBPASS
char_server_db: $DBNAME
char_server_ip: localhost
map_server_id: $DBUSER
map_server_pw: $DBPASS
map_server_db: $DBNAME
map_server_ip: localhost
log_db_id: $DBUSER
log_db_pw: $DBPASS
log_db_db: $DBNAME
log_db_ip: localhost
EOF
		[ $? -eq 0 ] || aborterror "Unable to import configuration, aborting tests."
		echo "Running 3CeAM with command line: ./map-server --run_once"
		cd rewrite
		./map-server_sql --run_once
		;;
esac



