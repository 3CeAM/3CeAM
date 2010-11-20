//(=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=)
//(       (c)2005 RagMods Modification Team presents        )
//(       ______  __    __                                  )
//(      /\  _  \/\ \__/\ \                     v 1.00.00   )
//(    __\ \ \_\ \ \ ,_\ \ \___      __    ___      __      )
//(  /'__`\ \  __ \ \ \/\ \  _ `\  /'__`\/' _ `\  /'__`\    )
//( /\  __/\ \ \/\ \ \ \_\ \ \ \ \/\  __//\ \/\ \/\ \_\.\_  )
//( \ \____\\ \_\ \_\ \__\\ \_\ \_\ \____\ \_\ \_\ \__/.\_\ )
//(  \/____/ \/_/\/_/\/__/ \/_/\/_/\/____/\/_/\/_/\/__/\/_/ )
//(                                                         )
//(            -- [s] [c] [r] [i] [p] [t] [s] --            )
//(   _   _   _   _   _   _   _     _   _   _   _   _   _   )
//(  / \ / \ / \ / \ / \ / \ / \   / \ / \ / \ / \ / \ / \  )
//( ( e | n | g | l | i | s | h ) ( A | t | h | e | n | a ) )
//(  \_/ \_/ \_/ \_/ \_/ \_/ \_/   \_/ \_/ \_/ \_/ \_/ \_/  )
//(                                                         )
//(  Advanced Fusion Maps (c) 2003-2005 The Fusion Project  )
//(=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=)
//===== eAthena Script ======================================
//= Mail System (using built in mail function)
//===========================================================
//===== By ==================================================
//= [Lance]
//===== Version =============================================
//= 1.0
//===== Compatible With =====================================
//= Any flavours of Athena SQL
//===== Description =========================================
//= Allows players to send and receive mails without GM lvl.
//===== Comments ============================================
//= 1.0 - Initial release                             [Lance]
//===========================================================
prontera,143,171,3	script	Messenger	738,1,1,{
	mes "[Messenger Deviruchi]";
	mes "Hiya! I'm the fastest messenger in Rune Midgard!";
	next;
	mes "[Messenger Deviruchi]";
	mes "I can send your friends messages even if he or she is offline!";
	next;
	mes "[Messenger Deviruchi]";
	mes "Wanna try?";
	menu "Yes", MENUSYS, "No", -;
	close;

MENUSYS:
	next;
	mes "[Messenger Deviruchi]";
	mes "What can I do for ya?";
	emotion e_what;
	menu "Check Mail",L_CHECK,"Send Mail",L_SEND, "Leave", -;
	next;
	mes "[Messenger Deviruchi]";
	mes "Do come again!";
	emotion e_no1;
	close;

L_CHECK:
	atcommand strcharinfo(0) + ":@listnewmail";
	menu "Read Mail", L_READ, "Check All Mails",CHKALLMAIL,"Send Mail",L_SEND, "Delete Mail", DELMAIL, "Back", MENUSYS;
	close;

CHKALLMAIL:
	atcommand strcharinfo(0) + ":@listmail";
	menu "Read Mail", L_READ, "Delete Mail", DELMAIL, "Back", MENUSYS;
	close;

L_READ:
	next;
	mes "[Messenger Deviruchi]";
	mes "Please tell me the message number you want to read.";
	input @msgnum;
	next;
	mes "[Messenger Deviruchi]";
	mes "Here it is!";
	emotion e_no1;
	atcommand strcharinfo(0) + ":@readmail " + @msgnum;
	menu "Reply Mail", L_SEND, "Back", MENUSYS;
	close;

L_SEND:
	next;
	mes "[Messenger Deviruchi]";
	mes "Who do you want to send this message to?";
	input @rcpt$;
	next;
	mes "[Messenger Deviruchi]";
	mes "What message do you want to send to him?";
	input @body$;
	next;
	atcommand strcharinfo(0) + ":@sendmail " + @rcpt$ + " " + @body$;
	mes "[Messenger Deviruchi]";
	mes "All done!";
	emotion e_no1;
	menu "Send another mail", L_SEND, "Back", MENUSYS;
	close;

DELMAIL:
	next;
	mes "[Messenger Deviruchi]";
	mes "Which message number do you want me to delete?";
	input @msgnum;
	next;
	mes "[Messenger Deviruchi]";
	mes "Are you sure you want to delete mail no. " + @msgnum + "?";
	menu "Yes",-,"No",DELMAIL, "Back", MENUSYS;
	atcommand strcharinfo(0) + ":@deletemail " + @msgnum;
	mes "[Messenger Deviruchi]";
	mes "All done!";

	emotion e_no1;
	menu "Delete another mail", DELMAIL, "Back", MENUSYS;
	close;

OnTouch:
	npctalk "Relax.. I'm no bad guy..";
	emotion e_heh;
	end;
}