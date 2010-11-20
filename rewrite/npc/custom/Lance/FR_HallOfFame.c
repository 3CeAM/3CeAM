//===== eAthena Script ======================================
//= Hall of Fame
//===========================================================
//===== By ==================================================
//= [Lance]
//= Idea from emilylee78
//===== Version =============================================
//= 2.4 FINAL
//===== Compatible With =====================================
//= eAthena SVN and Freya SVN
//===== Description =========================================
//= A Hall of Fame framework. Will update the list on every
//= login and logout in a safe manner.
//=
//= Usage: callfunc "printHallOfFame", $;
//= $ - Can be either 0 or 1.
//= 0 - Display the current rankings.
//= 1 - Display last week's rankings.
//= Note : Remember to put a close; after calling it.
//===== Comments ============================================
//= 1.0 - Initial beta release                      [Lance]
//= 1.1 - Fixed typos. Optimized a teeny bit.       [Lance]
//= 1.2 - Bug fixes.                                [Lance]
//= 1.3 - Added a more realistic shuffling.         [Lance]
//= 1.4 - Added Weekly Top 10 list.                 [Lance]
//= 1.5 - Friggin typos =< + Better shuffling       [Lance]
//= 1.6 - Bugfixes                                  [Lance]
//= 1.7 - More bugfixes. Type mismatch =P           [Lance]
//= 2.0 - Exclude GMs and add recovery plan.
//=       Suggested by EvilPoringOfDooom.           [Lance]
//= 2.1 - Typo.. again.. T_T                        [Lance]
//= 2.2 - Minor updates and added Jury              [Lance]
//= 2.3 - Utilizing eAthena's new scripting engine  [Lance]
//= 2.4 - Minor bug fix with event script label (bugport:722) [Samura22]
//===========================================================

prontera,0,0,0	script	OnPCLoginEvent	-1,{
OnPCLoginEvent:
	callfunc "HallOfFameInit";
	end;

OnInit:
	// Total Number of Players in Hall of Fame
	// =======================================
	set $HoF_totalCount, 10;
	// Reshuffle (Will affect perfomance) ====
	set $HoF_reshuffle, 1;
	// Minimum GM Lvl to be excluded from HoF=
	set $HoF_minGMLvl, 99;
	// Recovery Plan to Remove GMs ============
	set $@HoF_recovery, 0;
	// =======================================

	//set $HoF_totalCount, $HoF_totalCount - 1;
	set $@FebruaryD, 28;
	if((gettime(7) % 4) == 0) {
		set $@FebruaryD, 29;
	}
	setarray $@MonthDayThing[1],31, $@FebruaryD,31,30,31,30,31,31,30,31,30,31;
	set $@HoF_TimeUpdateD, $HoF_LastUpdateD;
	set $@HoF_TimeUpdateM, $HoF_LastUpdateM;
	set $@HoF_TimeUpdateY, $HoF_LastUpdateY;
	// Time to do some maths
	set $@TimeNowD, gettime(5);
	set $@TimeNowM, gettime(6);
	set $@TimeNowY, gettime(7);
	// Debug Message --
	debugmes "[Hall of Fame] Last Update is Year " + $@HoF_TimeUpdateY + " Month " + $@HoF_TimeUpdateM + " Day " + $@HoF_TimeUpdateD;
	debugmes "[Hall of Fame] Today is Year " + $@TimeNowY + " Month " + $@TimeNowM + " Day " + $@TimeNowD;
	if(($@TimeNowD - $@HoF_TimeUpdateD) < 0){
		set $@TimeNowD, $@TimeNowD + $@MonthDayThing[$@TimeNowM];
		set $@TimeNowM, $@TimeNowM - 1;
	}
	set $@GapD, $@TimeNowD - $@HoF_TimeUpdateD;
	if(($@TimeNowM - $@HoF_TimeUpdateM) < 0){
		set $@TimeNowM, $@TimeNowM + 12;
		set $@TimeNowY, $@TimeNowY - 1;
	}
	set $@GapM, $@TimeNowM - $@HoF_TimeUpdateM;
	set $@GapY, $@TimeNowY - $@HoF_TimeUpdateY;
	debugmes "[Hall of Fame] Gap is " + $@GapY + " Years " + $@GapM + " Months " + $@GapD + " Days.";
	if($@GapY > 0 || $@GapM > 0 || $@GapD >= 7) {
		callfunc "hallOfFameReset"; // Phew..
	}
	end;

OnClock0000:
	set $HoF_UpdateCount, $HoF_UpdateCount + 1;
	if($HoF_UpdateCount == 7) {
		callfunc "hallOfFameReset";
	}
	end;
}

prontera,0,0,0	script	PCLogoutEvent	-1,{
OnPCLogoutEvent:
	callfunc "HallOfFameInit";
	end;
}

function	script	hallOfFameReset	{
	copyarray $HoF_LadderNameO$[0], $HoF_LadderName$[0], $HoF_totalCount;
	copyarray $HoF_LadderBLevelO[0], $HoF_LadderBLevel[0], $HoF_totalCount; 
	copyarray $HoF_LadderJLevelO[0], $HoF_LadderJLevel[0], $HoF_totalCount;
	copyarray $HoF_LadderZenyO[0], $HoF_LadderZeny[0], $HoF_totalCount;
	deletearray $HoF_LadderName$[0], $HoF_totalCount;
	deletearray $HoF_LadderBLevel[0], $HoF_totalCount; 
	deletearray $HoF_LadderJLevel[0], $HoF_totalCount;
	deletearray $HoF_LadderZeny[0], $HoF_totalCount;
	set $HoF_LastUpdateD, gettime(5);
	set $HoF_LastUpdateM, gettime(6);
	set $HoF_LastUpdateY, gettime(7);
	set $HoF_UpdateCount, 0;
	debugmes "[Hall of Fame] System Reset Invoked!";
	return;
}

function	script	HallOfFameInit	{
	if(getgmlevel() >= $HoF_minGMLvl && $@HoF_recovery != 1) {
		set PCLogoutEvent, 0;
	} else {
		set PCLogoutEvent, 1;
		callfunc "updateHallofFame", $HoF_reshuffle;
	}
	return;
}

function	script	updateHallofFame	{
	set @i, 0;
	if(getarg(0) == 1){
		goto L_ShuffleName;
	}
	if(BaseLevel >= $HoF_LadderBLevel[$HoF_totalCount-1]){
		goto L_checkBase;
	}
	goto L_End;

L_ShuffleName:
	if($HoF_LadderName$[@i] == strcharinfo(0)) {
		goto L_ShuffleScore;
	}
	if(@i == $HoF_totalCount-1) {
		goto L_checkEntry;
	}
	set @i, @i + 1;
	goto L_ShuffleName;

L_ShuffleScore:
	deletearray $HoF_LadderName$[@i],1;
	deletearray $HoF_LadderZeny[@i],1;
	deletearray $HoF_LadderJLevel[@i],1;
	deletearray $HoF_LadderBLevel[@i],1;
	goto L_ShuffleName;

L_checkEntry:
	if(getgmlevel() >= $HoF_minGMLvl){
		end;
	}
	set @i, 0;
	goto L_checkBase;

L_checkBase:
	if(BaseLevel >= $HoF_LadderBLevel[@i]) {
		goto L_BaseOK;
	} else {
		goto L_Increment;
	}

L_BaseOK:
	if(BaseLevel == $HoF_LadderBLevel[@i]){
		goto L_BaseSameLoop;
	} else {
		goto L_NewEntry;
	}

L_BaseSameLoop:
	if(JobLevel >= $HoF_LadderJLevel[@i]) {
		goto L_JobOK;
	} else if(Zeny >= $HoF_LadderZeny[@i]) {
		goto L_ZenyOK;
	}
	goto L_Increment;

L_JobOK:
	if(JobLevel == $HoF_LadderJLevel[@i]) {
		goto L_JobSame;
	} else {
		goto L_NewEntry;
	}

L_ZenyOK:
	if(Zeny == $HoF_LadderZeny[@i]){
		goto L_Increment;
	} else {
		goto L_NewEntry;
	}

L_JobSame:
	if(Zeny >= $HoF_LadderZeny[@i]) {
		goto L_ZenyOK;
	} else {
		goto L_Increment;
	}

L_NewEntry:
	callfunc "hallOfFameNewEntry", @i, strcharinfo(0), BaseLevel, JobLevel, Zeny;
	end;

L_Increment:
	if(@i == $HoF_totalCount-1) {
		goto L_End;
	} else {
		set @i, @i + 1;
		goto L_checkBase;
	}

L_End:
	return;

}


function	script	hallOfFameNewEntry	{
	if(getarg(0) == 0) goto L_New_Entry;
	else if(getarg(1) != $HoF_LadderName$[getarg(0) - 1]) goto L_New_Entry;
	return;

L_New_Entry:
		set @startPos, getarg(0);
		copyarray @HoF_LadderNameB$[0], $HoF_LadderName$[@startPos], $HoF_totalCount;
		copyarray @HoF_LadderBLevelB[0], $HoF_LadderBLevel[@startPos], $HoF_totalCount; 
		copyarray @HoF_LadderJLevelB[0], $HoF_LadderJLevel[@startPos], $HoF_totalCount;
		copyarray @HoF_LadderZenyB[0], $HoF_LadderZeny[@startPos], $HoF_totalCount; 
		setarray $HoF_LadderName$[@startPos], getarg(1);
		setarray $HoF_LadderBLevel[@startPos], getarg(2);
		setarray $HoF_LadderJLevel[@startPos], getarg(3);
		setarray $HoF_LadderZeny[@startPos], getarg(4);
		set @startPos, @startPos + 1;
		set @limitPos, $HoF_totalCount - @startPos;
		copyarray $HoF_LadderName$[@startPos], @HoF_LadderNameB$[0], @limitPos;
		copyarray $HoF_LadderBLevel[@startPos], @HoF_LadderBLevelB[0], @limitPos; 
		copyarray $HoF_LadderJLevel[@startPos], @HoF_LadderJLevelB[0], @limitPos;
		copyarray $HoF_LadderZeny[@startPos], @HoF_LadderZenyB[0], @limitPos;
		announce "[Hall of Fame] " + getarg(1) + " has made his/herself onto the No. " + @startPos + " ranking in Hall of Fame!", bc_all;
		return;
}

function	script	printHallOfFame	{
	if(getarg(0) == 1) {
		mes "[Hall of Fame] - Last Week's Rankings";
		for(set @loop, 0; @loop < $HoF_totalCount; set @loop, @loop + 1){
			mes "^ff0000";
			mes "Position No. " + (@loop + 1) + ":^0000ff";
			mes "+================================+";
			mes "^000000Name :" + $HoF_LadderNameO$[@loop];
			mes "BLvl :" + $HoF_LadderBLevelO[@loop];
			mes "JLvl :" + $HoF_LadderJLevelO[@loop];
			mes "Zeny :" + $HoF_LadderZenyO[@loop] + "^0000ff";
			mes "+================================+^000000";
		}
	} else {
		mes "[Hall of Fame] - Current Rankings";
		for(set @loop, 0; @loop < $HoF_totalCount; set @loop, @loop + 1){
			mes "^ff0000";
			mes "Position No. " + (@loop + 1) + ":^0000ff";
			mes "+================================+";
			mes "^000000Name :" + $HoF_LadderName$[@loop];
			mes "BLvl :" + $HoF_LadderBLevel[@loop];
			mes "JLvl :" + $HoF_LadderJLevel[@loop];
			mes "Zeny :" + $HoF_LadderZeny[@loop] + "^0000ff";
			mes "+================================+^000000";
		}
	}
	return;
}

prontera,164,134,2	script	Jury	109,{
	mes "[Jury]";
	mes "Good day. Would you like to view the Hall of Fame?";
	next;
	menu "Yes",L_OK,"No",L_QUIT;

L_OK:
	mes "[Jury]";
	mes "Would you like to view the current or the past rankings?";

L_MENU:
	next;
	menu "Current", L_CUR, "Past", -,"Nevermind",L_QUIT;
	callfunc "printHallOfFame",1;
	goto L_MENU;

L_CUR:
	callfunc "printHallOfFame",0;
	goto L_MENU;

L_QUIT:
	mes "[Jury]";
	mes "Have a nice day then.";
	close;
	
}
