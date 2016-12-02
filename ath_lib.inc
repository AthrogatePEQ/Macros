| |--------------------------------------------------------|
| |- Searches inventory, and banks, for indicated items.	-|
| |--------------------------------------------------------|
| |- Use, /bc Find 'Name', or, /echo Find 'Name'.			-|
| |--------------------------------------------------------|
| #EVENT FindItemCount "<#1#> Count Item #2#"
| #EVENT FindItemCount "<#1#> Find Item Count #2#"
| SUB EVENT_FindItemCount(line, ChatSender, eventParams)
| |/if (${Debug} || ${Debug_Count}) /echo |- EVENT_FindItemCount ==>
| |/if (${Debug} || ${Debug_Count}) /echo line: ${line}
| |/if (${Debug} || ${Debug_Count}) /echo ChatSender: ${ChatSender}
| |/if (${Debug} || ${Debug_Count}) /echo eventParms: ${eventParams}
| 
| 	| -Verify the event
| 	/call verifyEvent "${ChatSender}" "${line}"
| 	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
| 	|    |-----------Valid User----------|    |--------Included Bot-----------|
| 	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {
| 	
| 		| -Declare counting variables.
| 		/declare i int local
| 		/declare e int local
| 		/declare itemName string local
| 		/declare ItemToFind string local ${eventParams.Arg[1,-]}
| 		/declare OnlyInventory bool local FALSE
| 				
| 		/if (${line.Find[/Inventory]}) /varset OnlyInventory TRUE
| 		
| 		| Remove /only /not /otherthings if needed		
| 		/if (${ItemToFind.Find[ /]}) /varset ItemToFind ${ItemToFind.Left[${Math.Calc[${ItemToFind.Find[ /]} - 1]}]}				
| 		
| 		/if (${Debug} || ${Debug_Count}) /echo ItemToFind: [${ItemToFind}] OnlyInventory: ${OnlyInventory}
| 
| 		/if (${OnlyInventory} ) {
| 			/if ( ${FindItemCount[${ItemToFind}]} ) {
| 				/docommand ${ChatToggle} >>> Found (${Math.Calc[${FindItemCount[${ItemToFind}]}].Int}) examples of [${ItemToFind}] in the following slots ... <<<
| 			}
| 		} else /if (${FindItemCount[${ItemToFind}]} || ${FindItemBankCount[${ItemToFind}]} || ${Cursor.Name.Equal[${ItemToFind}]}) {			
| 			/docommand ${ChatToggle} >>> Found (${Math.Calc[${FindItemCount[${ItemToFind}]} + ${FindItemBankCount[${ItemToFind}]}].Int}) examples of [${ItemToFind}] in the following slots ... <<<		
| 		}
| 	}
| 
| /if (${Debug} || ${Debug_Count}) /echo <== EVENT_FindItems -|
| /RETURN

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

|###################################################################
| Raid Buff
| Subroutine to hopefully speed up buffing for mobilization
|
|###################################################################
#event raidbuff "<#1#> Raidbuff#2#"
Sub Event_raidbuff(line, ChatSender, eventParms)
/if (${Debug}) /echo |- Event_raidbuff ==>
	| -Verify the event
  /call #verifyEvent "${ChatSender}" "${line}"
  /if (${Macro.Return}) {
		/if (${Me.AltAbility[Mass Group Buff].ID} && ${Me.AltAbilityReady[Mass Group Buff]} ) {
			/if ( ${MassGroupBuff.Length} > 0) {
				/actionrequest MGB Regen "${MassGroupBuff.Arg[1,/]}"
				/delay 50 ${ActionLock.Request}
				/if (${ActionLock.Proceed}) {
					/call e3_Cast ${Me.ID} "Mass Group Buff"
					/bc Casting "${MassGroupBuff.Arg[1,/]}"
					/call e3_Cast ${ChatSender} "${MassGroupBuff}/giveuptimer|45s"
				}
			}
		}
	}

	| old junk
	|/echo /bct ${Buffie} //tell ${Buffer} "${BuffName}"
	|/bct ${Buffie} //tell ${Buffer} "${BuffName}"

/if (${Debug}) /echo <== Event_raidbuff -|

/return



#EVENT FindItemCount "<#1#> Find Item Count #2#"
SUB EVENT_FindItemCount(line, ChatSender, args)

  /if (${Debug} || ${Debug_Count}) /echo |- EVENT_FindItemCount ==>
  |/if (${Debug} || ${Debug_Count}) /echo line: ${line}
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


#EVENT ReportMana "[#1#] Mana Check#*#"
#EVENT ReportMana "<#1#> Mana Check#*#"
Sub Event_ReportMana(line, ChatSender, eventParams)
|/varset Debug 1
/if (${Debug}) /echo |- Event_ReportMana ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		/if (!${Select[${Me.Class.ShortName}, "WAR","MNK","ROG","BER"]}) {
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

