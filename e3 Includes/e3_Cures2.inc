#event Cure2Toggle "<#1#> Cure2 Auto Toggle #2#"
sub event_Cure2Toggle(line, ChatSender, args)
|/echo [[ ENTERING Cure2Toggle ]] ChatSender: ${ChatSender} args: ${args}

	/call #verifyEvent "${ChatSender}" "${line}"
	/if (!${Macro.Return}) /return

	/if (!${Defined[Cure2Autocure]}) /declare Cure2Autocure bool outer false
	/if (!${Defined[autoRCure]}) /declare autoRCure bool outer false

	/declare mystring string local ${args.Arg[1, ]}

	/if (${mystring.Equal[on]}) {
		/varset Cure2Autocure TRUE
		/varset autoRCure TRUE
	} else /if (${mystring.Equal[off]}) {
		/varset Cure2Autocure FALSE
		/varset autoRCure FALSE
	}

	/docommand ${ChatToggle} Cure2 Auto Curing: [${If[${Cure2Autocure},ON,OFF]}]

|/echo [[ Leaving  Cure2Toggle ]]
/return

#event Cure2 "<#1#> Do Cure2#2#"
sub event_Cure2(line, ChatSender, args)
|/echo [[ ENTERING CURE2 ]] autoRCure: ${autoRCure} doCures2: ${doCures2}

	/call #verifyEvent "${ChatSender}" "${line}"
	/if (!${Macro.Return}) /return

| 1- checks Radiant Curing: AutoRadiant + RadiantCure#
  /if (${autoRCure}) /call do_RadiantCure

| 2- checks specified Target curing: Cure#
  /if (!${ActionTaken} && ${doCures2}) {
		/call do_Cures2
	}

|/echo [[ LEAVING CURE2 ]]
/return

sub check_cures2
	/if (!${Defined[Cure2Autocure]}) /declare Cure2Autocure bool outer false
	/if (!${Defined[autoRCure]}) /declare autoRCure bool outer false

	/if (${autoRCure}) /call do_RadiantCure

	/if (!${ActionTaken} && ${Cure2Autocure}) /call do_Cures2

/return

sub do_Cures2
	/call do_Cures2Group
	/call do_Cure2Netbots
/return

sub do_Cures2Group
|/echo do_MyCuresGroup
	/declare i int local 0
	/declare n int local 0

	/for n 0 to ${Group}
		/if (${Bool[${Spawn[pc =${Group.Member[${n}]}]}]}) {
  		/call athlib_process_detramentals ${Group.Member[${n}]}
		}
	/next n
/return

sub do_Cure2Netbots
|/echo do_MyCuresNetbots

	/declare n int local 0
	/declare myCheckMember string local
	/declare myCheckID     int    local
	/for n 0 to ${NetBots.Counts}
		/if (${SpawnCount[pc =${NetBots.Client[${n}]}]} ) {
			/varset myCheckMember ${NetBots.Client[${n}]}
			/varset myCheckID     ${Spawn[${NetBots.Client[${n}]}].ID}
			/call athlib_process_detramentals ${myCheckMember}
		}
	/next n
/return

sub athlib_process_detramentals(targetName)
	/declare i int local 0
	/declare continue bool local false

	/varset continue FALSE
	/for i 0 to 20
:Cure2CureBot
|/echo ${i} ${targetName}
		/if (${Bool[${NetBots[${targetName}].Buff[${i}]}]}) {
			/if (${NetBots[${targetName}].Buff[${i}].CounterType.Equal[Poison]} && ${Bool[${Me.Book[${CurePoisonSpell}]}]}) {
				/call cure2_cast_cure_spell "${targetName}" "${CurePoisonSpell}"
				/if (${Bool[${Macro.Return}]}) /varset continue TRUE
				/if (${Bool[${Macro.Return}]}) /goto :Cure2CureBot
			} else /if (${NetBots[${targetName}].Buff[${i}].CounterType.Equal[Disease]} && ${Bool[${Me.Book[${CureDiseaseSpell}]}]}) {
				/call cure2_cast_cure_spell "${targetName}" "${CureDiseaseSpell}"
				/if (${Bool[${Macro.Return}]}) /varset continue TRUE
				/if (${Bool[${Macro.Return}]}) /goto :Cure2CureBot
			} else /if (${NetBots[${targetName}].Buff[${i}].CounterType.Equal[Curse]} && ${Bool[${Me.Book[${CureCurseSpell}]}]}) {
				/call cure2_cast_cure_spell "${targetName}" "${CureCurseSpell}"
				/if (${Bool[${Macro.Return}]}) /varset continue TRUE
				/if (${Bool[${Macro.Return}]}) /goto :Cure2CureBot
			}
		}
	/next i

/return ${continue}

sub cure2_cast_cure_spell(cureTarget, cureSpell)
	/declare targetID int local ${Spawn[pc =${cureTarget}].ID}
	/if (!${Bool[${targetID}]}) /return FALSE

	/call check_Distance ${targetID} ${Spell[${cureSpell}].MyRange}
	/if (!${Macro.Return}) /return FALSE

	/casting "${cureSpell}" ${DefaultGem} "-targetid|${targetID}" -maxtries|3
	/delay 6s ${Cast.Ready[${cureSpell}]}

/return TRUE

Sub Cures2_Setup

	/declare Cure2Autocure bool outer false

	/call #IniToVar "${Character_Ini},Cures,CurePoison" 	CurePoisonSpell string outer
 	/call #IniToVar "${Character_Ini},Cures,CureDisease" 	CureDiseaseSpell string outer
 	/call #IniToVar "${Character_Ini},Cures,CureCurse" 		CureCurseSpell string outer

 	/declare doCures2 bool outer FALSE
	/if (!${Defined[autoRCure]}) /declare autoRCure bool outer false

 	/if (${Bool[${CurePoisonSpell}]} || ${Bool[${CureDiseaseSpell}]} || ${Bool[${CureCurseSpell}]}) {
 		/varset doCures2 TRUE
 	}
/return

SUB Cures2_CharacterSettings
  /if (${Select[${Me.Class.ShortName},DRU,SHM,CLR,PAL,BST,RNG]}) {
  	/call WriteToIni "${Character_Ini},Cures,AutoCure (On/Off),Off"
    /call WriteToIni "${Character_Ini},Cures,CurePoison"
    /call WriteToIni "${Character_Ini},Cures,CureDisease"
    /call WriteToIni "${Character_Ini},Cures,CureCurse"
  }
/RETURN

Sub Cures2_MacroSettings
/return

Sub Cures2_aliases
	/squelch /alias /cure /bc do cure2
	/squelch /alias /autocure /bc Cure2 Auto Toggle
/return

Sub Cures2_Background_Events
	/doevents Cure2
	/doevents Cure2Toggle
/return
