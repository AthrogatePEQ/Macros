#event mgbregen "<#1#> MGB Regen"
sub event_mgbregen(line, ChatSender, args)
/if (${Debug} || ${Debug_Count}) /echo |- EVENT_mgbregen ==>
  /call #verifyEvent "${ChatSender}" "${line}"
  /if (${Macro.Return}) {
    /if (${Me.AltAbility[Mass Group Buff].ID} && ${Me.AltAbilityReady[Mass Group Buff]} ) {
			/if (${Ini[${Character_Ini},Heals,MGB Regen].Length}) {
				/call IniToVar "${Character_Ini},Heals,MGB Regen" MGBRegen string outer

				/if ( ${Me.AltAbility[${MGBRegen}].ID} && ${Me.AltAbilityReady[${MGBRegen}]}) {

					/actionrequest MGB Regen ${Me.Class}
					/delay 50 ${ActionLock.Request}
					/if (${ActionLock.Proceed}) {
						/bc Mass Group Buffing ${MGBRegen}
						/casting "Mass Group Buff" -maxtries|3
						/delay 10
						/casting "${MGBRegen}" -maxtries|3
					}
				}
			}
    }
  }
/if (${Debug} || ${Debug_Count}) /echo <== EVENT_mbgregen |
/return

#event manaburn "<#1#> Manaburn on #2#"
sub event_manaburn(line, ChatSender, int mobID)
/if (${Debug} || ${Debug_Count}) /echo |- EVENT_manaburn ChatSender: ${ChatSender} mobID: ${mobID}==>
  /call #verifyEvent "${ChatSender}" "${line}"
  /if (${Macro.Return}) {

		/if (${Ini[${Character_Ini},Burn,Mana Burn].Length}) {
			/call IniToVar "${Character_Ini},Burn,Mana Burn" Manaburn string outer

			/if ( ${Me.AltAbility[${Manaburn}].ID} && ${Me.AltAbilityReady[${Manaburn}]}) {

				/varset AssistTarget ${mobID}

				/if (${Target.ID} != ${AssistTarget}) /call TrueTarget ${AssistTarget}

				/actionrequest ${Me.Class.ShortName}_Manaburn
				/delay 50 ${ActionLock.Request}
				/if (${ActionLock.Proceed}) {
					/call validateCast "${AssistTarget}|NULL" "${Manaburn}"
					/if (${Macro.Return}) {
						/call e3_Cast ${AssistTarget} "${Manaburn}/GiveUpTimer|50"
						/delay 50 ${ActionTaken}
						/bc ${Manaburn} Used.
					}

				}
			}
		}

  }
/if (${Debug} || ${Debug_Count}) /echo <== EVENT_manaburn |
/return

#EVENT FindItemCount "<#1#> Find Item Count #2#"
SUB EVENT_FindItemCount(line, ChatSender, args)

  /if (${Debug} || ${Debug_Count}) /echo |- EVENT_FindItemCount ==>
  /if (${Debug} || ${Debug_Count}) /echo line: ${line}
  /if (${Debug} || ${Debug_Count}) /echo ChatSender: ${ChatSender}
  /if (${Debug} || ${Debug_Count}) /echo eventParms: ${args}
   
  | -Verify the event
  /call #verifyEvent "${ChatSender}" "${line}"
  
  /if (${Macro.Return}) {
  
    /call #CreateVars string outer FindItemPart FindItemArgs
    /call #SplitInclusionArgs "${args}" FindItemPart FindItemArgs "/Inventory"

    | -Declare counting variables.
    /declare i int local
    /declare e int local
    /declare itemName string local
    /declare ItemToFind string local ${FindItemPart}
    /declare OnlyInventory bool local FALSE
   
    /if (${FindItemArgs.Find[/Inventory]}) /varset OnlyInventory TRUE

    /if (${Debug} || ${Debug_Count}) /echo ItemToFind: [${ItemToFind}] OnlyInventory: ${OnlyInventory}

	/if (${OnlyInventory} ) {
		/if ( ${FindItemCount[${ItemToFind}]} ) {
			/docommand ${ChatToggle} >>> Found (${Math.Calc[${FindItemCount[${ItemToFind}]}].Int}) examples of [${ItemToFind}] in the following slots ... <<<
		}
	} else /if (${FindItemCount[${ItemToFind}]} || ${FindItemBankCount[${ItemToFind}]} || ${Cursor.Name.Equal[${ItemToFind}]}) {			
		/docommand ${ChatToggle} >>> Found (${Math.Calc[${FindItemCount[${ItemToFind}]} + ${FindItemBankCount[${ItemToFind}]}].Int}) examples of [${ItemToFind}] in the following slots ... <<<		
	}
  }
  /if (${Debug} || ${Debug_Count}) /echo <== EVENT_FindItems -|
  
/RETURN

|#event YourKill "You have slain #*#!"
#event MyKill "You gain experience!!"
|#event YourKill "You gain party experience!!"
|#event YourKill "You gained raid experience!"
SUB Event_MyKill
/if (${Debug} || ${Debug_Basics}) /echo ==> Event_YourKill -|

	|/if (${Me.Pet.ID}) {
	|	/pet hold
	|}
	/if ( !${Bool[${Group}]} ) {
		/bc I KSed the last mob
	}

/if (${Debug} || ${Debug_Basics}) /echo <== Event_YourKill -|
/RETURN

|#EVENT NOLONGERIDLE "You are no longer idle."
|SUB Event_NOLONGERIDLE
|	/removebuff Illusion: /ONLY|ME
|/RETURN

#EVENT raid_invite "#1# tells you, 'Raid Invite#2#'"
SUB EVENT_raid_invite(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_raid_invite ==>
	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	|/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	      |-----------In Zone-------------|
	/if ( ${Bool[${Macro.Return.Arg[4,_]}]} ) {
		/raidinvite ${ChatSender}
	} else {
		/tell ${ChatSender} We aren't in the same zone.
	}
/if (${Debug}) /echo <== EVENT_raid_invite -|
/return

#EVENT SERVER_REBOOT "#1#BROADCASTS, 'The server will be rebooting in 10 minutes. Please get to a safe location.#*#"
SUB EVENT_SERVER_REBOOT(line, ChatSender, eventParms)

  /if (!${Me.XTarget}) {
		/echo ${ChatSender} issued a server reboot.  Camping toons out.
		/camp desk
		/end
	}
/RETURN

#event report_aas "#1# REPORT AAS#*#"
sub event_report_aas(line, ChatSender, eventParms)

|	/declare myArray[26] local
|/call #CreateArrays string outer CharInfoWithLesson[${counter}] CharInfoNoLesson[${Math.Calc[${ExpCharInfo.Size}-${counter}]}]
|/call #CreateArrays string outer myArray[26]
/call #CreateArrays string outer myArray[${Math.Calc[${NetBots.Client.Count[ ]}+1]}]
		/echo ${NetBots.Client}
		/echo ${Math.Calc[${NetBots.Client.Count[ ]}+1]}
		/declare i int
		/for i 1 to ${Math.Calc[1+${NetBots.Client.Count[ ]}]} 
			|/echo ${NetBots.Client.Arg[${i}, ]}: ${NetBots[${NetBots.Client.Arg[${i}, ]}].TotalAA} - ${NetBots[${NetBots.Client.Arg[${i}, ]}].UsedAA} - ${NetBots[${NetBots.Client.Arg[${i}, ]}].UnusedAA} 
			|/declare myArray[${i}]
			/varset myArray[${i}] ${NetBots.Client.Arg[${i}, ]}|${NetBots[${NetBots.Client.Arg[${i}, ]}].TotalAA}|${NetBots[${NetBots.Client.Arg[${i}, ]}].UsedAA}|${NetBots[${NetBots.Client.Arg[${i}, ]}].UnusedAA}
		/next i
		
		/call #QuickSort myArray 1 ${myArray.Size} .Arg[2,|]
		
		/for i ${myArray.Size} downto 1
			/echo ${myArray[${i}]}
		/next i
/return

#event leveledup "Welcome to level #1#"
sub event_leveledup(line, level)
	/bc Level ${level}
/return

|--------------------------------------------------------------------------------------------------------------------
| Coth Functions
| 1) Event "COTH Raid"
| 2) Logical Assumptions:
| 	a) If there is a Mage in my group
|	b) If the Mage is > 200 (some arbitrary distance)
|	c) If "I" am not a mage.
|       THEN: /g coth (to fire the mage auto-summoning)
|
|--------------------------------------------------------------------------------------------------------------------
#EVENT CothGroup "<#*#> Coth group"
Sub Event_CothGroup

	/declare i int local
	/declare groupNum int local		

	/declare groupList string local
	/varset  groupList ${Me.GroupList}
	/varset i 0
	
	/declare groupMember 	  string local
	/declare groupMemberClass string local
	/declare groupMemberDist  int 	 local
	
	/echo ${groupList}
	
	:nextArg
	/if (${groupList.Find[ ]}) {
		
		/varset groupMember 	 ${groupList.Arg[${i}, ]}
		/varset groupMemberClass ${Spawn[pc =${groupMember}].Class.ShortName}
		/varset groupMemberDist  ${Spawn[pc =${groupMember}].Distance}
		
		/if (${groupMemberClass.Equal[MAG]} && ${groupMemberDist} > 200) {
			/echo groupMember: ${groupMember} is a ${groupMemberClass} and is ${groupMemberDist} away.
			| Found a Mage; CALL the HERO in for duty.
			/g coh
		} else {
			/varcalc i ${i} + 1
			/if (${groupList.Arg[${i}, ].Length}) /goto :nextArg
		}
	} else {
		|/if (${givenData.Arg[${i}, ].Find[${WhatToFind}]}) {
		|
		|	/varset foundData ${givenData.Arg[${i}, ].Arg[2,|]}
		|	/if (${Debug}) /echo |- argueString -| Found [${WhatToFind}] - [${foundData}]
		|
		|} else {
		|	/varcalc i ${i} + 1
		|	/if (${givenData.Arg[${i}, ].Length}) /goto :nextArg
		|}
	}

/RETURN

#EVENT COTHHappened "#1# steps into a mystic portal."
Sub Event_COTHHappened(line, ChatSender)
	/if ( ${Select[${Raid.Leader},${Me}]} ) {
		/bc ${ChatSender} has been Summoned.
	}
/RETURN

#Event SkillUp "You have become better at #1#! (#2#)"
Sub Event_SkillUp(String string1, String SkillName, String SkillNumber) 
	/if (!${Select[${SkillName},NULL]}) { 
		/bc ${SkillName}=${SkillNumber}
	}
/return

#EVENT ReportMana "<#1#> Mana Check#*#"
Sub Event_ReportMana(line, ChatSender, eventParams)
|/varset Debug 1
/if (${Debug}) /echo |- Event_ReportMana ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		/if (!${Select[${Me.Class.ShortName}, "WAR","MNK","ROG","BER"]} && ${Me.PctMana} < 100) {
			/if ( ${line.Find[raid]} ) {
				/docommand /timed 1 /rs Current Mana: ${Me.PctMana}%
			} else /if ( ${line.Find[group]} ) {
				/docommand /timed 1 /gsay Current Mana: ${Me.PctMana}%
			} else /if ( ${line.Find[guild]} ) {
				/docommand /timed 1 /gu Current Mana: ${Me.PctMana}%
			} else /if ( ${line.Find[tells you]} ) {
				/docommand /timed 1 /reply Current Mana: ${Me.PctMana}%			
			} else {			
				/docommand /timed 1 ${ChatToggle} Current Mana: ${Me.PctMana}%
			}
		}
	}
	
/if (${Debug}) /echo <== Event_ReportMana -|
|/varset Debug FALSE

/return


#EVENT FindItemWithID "<#1#> FindItemWithID #2# #3#"
SUB Event_FindItemWithID(line, ChatSender, args, args2)
/if (${Debug}) /echo Event_FindItemWithID ==> -|
  | -Verify the event
  /call #verifyEvent "${ChatSender}" "${line}"
  
  /if (${Macro.Return}) {

    /if (${Debug}) /echo Event_FindItemWithID BEGIN

    /call #CreateVars string outer FindItemPart FindItemArgs
    /call #SplitInclusionArgs "${args}" FindItemPart FindItemArgs "/Inventory"

    | -Declare counting variables.
    /declare i int local
    /declare e int local
    /declare itemName string local
    /declare ItemIDToFind string local ${FindItemPart}
    /declare ItemToFind string local ${args2}
    /declare OnlyInventory bool local FALSE
  
    /if (${Debug}) /echo ItemIDToFind ${ItemIDToFind} 
    /if (${Debug}) /echo ItemToFind ${ItemToFind}
    
    /if (${FindItemCount[${ItemToFind}]} || ${FindItemBankCount[${ItemToFind}]} || ${Cursor.Name.Equal[${ItemToFind}]}) {      
      /if ( ${FindItem[=${ItemToFind}].ID} == ${ItemIDToFind}) {
        /bc >>> Found ${ItemToFind} with ItemID: ${ItemIDToFind} 
      }
      |/bc >>> Found (${Math.Calc[${FindItemCount[${ItemToFind}]} + ${FindItemBankCount[${ItemToFind}]}].Int}) examples of [${ItemToFind}] in the following slots ... <<<   
    }
    /if (${Debug}) /echo Event_FindItemWithID END
  }
/if (${Debug}) /echo <== Event_FindItemWithID -|
/return

#EVENT WhoNeedsTwo "<#1#> WhoNeeds2 #2# #3#"
SUB Event_WhoNeedsTwo(line, ChatSender, arg1, arg2)
  /if ((${Math.Calc[${FindItemCount[=${arg1}]} + ${FindItemBankCount[=${arg1}]}]})==0) {
    /bc 0
  } else {
    /if ( !${FindItem[=${arg2}].ID} == ${arg1}) /bc 0
  }
  |/if ((${Math.Calc[${FindItemCount[=${arg1}]} + ${FindItemBankCount[=${arg1}]}]})==0) /bc 0
/return

#event MyCure "<#1#> MyCure#2#"
sub event_MyCure(line, ChatSender, args)
/echo [[ ENTERING MY CURE ]]
/echo autoRCure: ${autoRCure}
/echo doMyCures: ${doMyCures}

	/call #verifyEvent "${ChatSender}" "${line}"
	/if (!${Macro.Return}) /return

| 1- checks Radiant Curing: AutoRadiant + RadiantCure#
  /if (${autoRCure}) /call do_RadiantCure

| 2- checks specified Target curing: Cure#
  /if (!${ActionTaken} && ${doMyCures}) {
		/call do_MyCures
	}

/echo [[ LEAVING MY CURE ]]
/return

sub do_MyCures
	/call do_MyCuresGroup
	/call do_MyCureNetbots
	/echo actionTaken: ${ActionTaken}
/return

sub do_MyCuresGroup
/echo do_MyCuresGroup
	/declare i int local 0
	/declare n int local 0

|1 check group
	/for n 0 to ${Group}
		/if (${Bool[${Spawn[pc =${Group.Member[${n}]}]}]}) {
		/call athlib_process_detramentals ${Group.Member[${n}]}
|			/for i 0 to 20
|:CureBot
|				/if ( ${Bool[${NetBots[${Group.Member[${n}]}].Buff[${i}]}]} ) {
|
|					/if ( ${NetBots[${Group.Member[${n}]}].Buff[${i}].SpellType.Equal[Detrimental]} && ${Bool[${NetBots[${Group.Member[${n}]}].Buff[${i}].CounterType}]} ) {
|/echo "${n} ${Group.Member[${n}]} ${NetBots[${Group.Member[${n}]}].Buff[${i}]}" "${NetBots[${Group.Member[${n}]}].Buff[${i}].CounterType}"
|
|						/if (${NetBots[${Group.Member[${n}]}].Buff[${i}].CounterType.Equal[Poison]} && ${Bool[${Me.Book[${CurePoisonSpell}]}]}) {
|							/call athlib_cure_spell "${Group.Member[${n}]}" "${CurePoisonSpell}"
|							/if (${Bool[${NetBots[${Group.Member[${n}]}].Buff[${i}]}]} && ${NetBots[${Group.Member[${n}]}].Buff[${i}].CounterType.Equal[Poison]}) /goto :CureBot
|						} else /if (${NetBots[${Group.Member[${n}]}].Buff[${i}].CounterType.Equal[Disease]} && ${Bool[${Me.Book[${CureDiseaseSpell}]}]}) {
|							/call athlib_cure_spell "${Group.Member[${n}]}" "${CureDiseaseSpell}"
|							/if (${Bool[${NetBots[${Group.Member[${n}]}].Buff[${i}]}]} && ${NetBots[${Group.Member[${n}]}].Buff[${i}].CounterType.Equal[Disease]}) /goto :CureBot
|						} else /if (${NetBots[${Group.Member[${n}]}].Buff[${i}].CounterType.Equal[Curse]} && ${Bool[${Me.Book[${CureCurseSpell}]}]}) {
|							/call athlib_cure_spell "${Group.Member[${n}]}" "${CureCurseSpell}"
|							/if (${Bool[${NetBots[${Group.Member[${n}]}].Buff[${i}]}]} && ${NetBots[${Group.Member[${n}]}].Buff[${i}].CounterType.Equal[Curse]}) /goto :CureBot
|						}
|					}
|				}
|			/next i
		}
	/next n
/return

sub do_MyCureNetbots
/echo do_MyCuresNetbots

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
:AthLibCureBot
|/echo ${i} ${targetName}
		/if (${Bool[${NetBots[${targetName}].Buff[${i}]}]}) {
			/if (${NetBots[${targetName}].Buff[${i}].CounterType.Equal[Poison]} && ${Bool[${Me.Book[${CurePoisonSpell}]}]}) {
				/call athlib_cure_spell "${targetName}" "${CurePoisonSpell}"
				|/if (${Bool[${NetBots[${targetName}].Buff[${i}]}]} && ${NetBots[${targetName}].Buff[${i}].CounterType.Equal[Poison]}) /goto :AthLibCureBot
				/if (${Bool[${Macro.Return}]}) /varset continue TRUE
				/if (${Bool[${Macro.Return}]}) /goto :AthLibCureBot
			} else /if (${NetBots[${targetName}].Buff[${i}].CounterType.Equal[Disease]} && ${Bool[${Me.Book[${CureDiseaseSpell}]}]}) {
				/call athlib_cure_spell "${targetName}" "${CureDiseaseSpell}"
				|/if (${Bool[${NetBots[${targetName}].Buff[${i}]}]} && ${NetBots[${targetName}].Buff[${i}].CounterType.Equal[Disease]}) /goto :AthLibCureBot
				/if (${Bool[${Macro.Return}]}) /varset continue TRUE
				/if (${Bool[${Macro.Return}]}) /goto :AthLibCureBot
			} else /if (${NetBots[${targetName}].Buff[${i}].CounterType.Equal[Curse]} && ${Bool[${Me.Book[${CureCurseSpell}]}]}) {
				/call athlib_cure_spell "${targetName}" "${CureCurseSpell}"
				|/if (${Bool[${NetBots[${targetName}].Buff[${i}]}]} && ${NetBots[${targetName}].Buff[${i}].CounterType.Equal[Curse]}) /goto :AthLibCureBot
				/if (${Bool[${Macro.Return}]}) /varset continue TRUE
				/if (${Bool[${Macro.Return}]}) /goto :AthLibCureBot
			}
		}
	/next i

/return ${continue}

sub athlib_cure_spell(cureTarget, cureSpell)
	/declare targetID int local ${Spawn[pc =${cureTarget}].ID}
	/if (!${Bool[${targetID}]}) /return FALSE

	/call check_Distance ${targetID} ${Spell[${cureSpell}].MyRange}
	/if (!${Macro.Return}) /return FALSE

	/casting "${cureSpell}" ${DefaultGem} "-targetid|${targetID}" -maxtries|3
	/delay 6s ${Cast.Ready[${cureSpell}]}

/return TRUE

#EVENT HealAt "<#1#> HealAt #2# #3#"
Sub event_HealAt(line, ChatSender, var1, var2)
	/call verifyEvent "${ChatSender}" "${line}"
	|    |-----------Valid User----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		/if (${Defined[${var1}]}) {
			/bc Setting ${var1} to heal at ${var2}
			/varset ${var1}[1,18] ${var2}
		}
	}
/return

Sub athlib_Setup
	/tgb on
	/call BlockBuffsAtStartup

	/call #IniToVar "${Character_Ini},Cures,CurePoison" 	CurePoisonSpell string outer
 	/call #IniToVar "${Character_Ini},Cures,CureDisease" 	CureDiseaseSpell string outer
 	/call #IniToVar "${Character_Ini},Cures,CureCurse" 		CureCurseSpell string outer

 	/declare doMyCures bool outer FALSE

 	/if (${Bool[${CurePoisonSpell}]} || ${Bool[${CureDiseaseSpell}]} || ${Bool[${CureCurseSpell}]}) {
 		/varset doMyCures TRUE
 	}
/return

SUB athlib_CharacterSettings
  /if (${Select[${Me.Class.ShortName},DRU,SHM,CLR,PAL,BST,RNG]}) {
    /call WriteToIni "${Character_Ini},Cures,CurePoison"
    /call WriteToIni "${Character_Ini},Cures,CureDisease"
    /call WriteToIni "${Character_Ini},Cures,CureCurse"
  }
/RETURN

Sub athlib_MacroSettings
/return

Sub athlib_aliases
	/alias /fic /bc find item count
	/noparse /alias /leader /noparse /bcaa //if (${Select[${Group.Leader},${Me}]})
	/noparse /alias /port /noparse /bcaa //if (${Select[${Group.Leader},${Me}]}) /g port
	/squelch /alias /expmode /bc ExpGroup
/return

Sub athlib_Background_Events
	/doevents ReportMana
	/doevents FindItemCount
	/doevents SkillUp
	/doevents raid_invite
	/doevents updateAliases
	/doevents WhoNeedsTwo
	/doevents report_aas
	/doevents MyCure
	/doevents HealAt
/return
