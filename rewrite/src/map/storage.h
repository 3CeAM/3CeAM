// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef _STORAGE_H_
#define _STORAGE_H_

//#include "../common/mmo.h"
struct storage_data;
struct guild_storage;
struct item;
//#include "map.h"
struct map_session_data;

int storage_delitem(struct map_session_data* sd, int n, int amount);
int storage_storageopen(struct map_session_data *sd);
int storage_storageadd(struct map_session_data *sd,int index,int amount);
int storage_storageget(struct map_session_data *sd,int index,int amount);
int storage_storageaddfromcart(struct map_session_data *sd,int index,int amount);
int storage_storagegettocart(struct map_session_data *sd,int index,int amount);
void storage_storageclose(struct map_session_data *sd);
int do_init_storage(void);
void do_final_storage(void);
void do_reconnect_storage(void);
void storage_storage_quit(struct map_session_data *sd, int flag);

struct guild_storage* guild2storage(int guild_id);
int guild_storage_delete(int guild_id);
int storage_guild_storageopen(struct map_session_data *sd);
int guild_storage_additem(struct map_session_data *sd,struct guild_storage *stor,struct item *item_data,int amount);
int guild_storage_delitem(struct map_session_data *sd,struct guild_storage *stor,int n,int amount);
int storage_guild_storageadd(struct map_session_data *sd,int index,int amount);
int storage_guild_storageget(struct map_session_data *sd,int index,int amount);
int storage_guild_storageaddfromcart(struct map_session_data *sd,int index,int amount);
int storage_guild_storagegettocart(struct map_session_data *sd,int index,int amount);
int storage_guild_storageclose(struct map_session_data *sd);
int storage_guild_storage_quit(struct map_session_data *sd,int flag);
int storage_guild_storagesave(int account_id, int guild_id, int flag);
int storage_guild_storagesaved(int guild_id); //Ack from char server that guild store was saved.

#endif /* _STORAGE_H_ */
