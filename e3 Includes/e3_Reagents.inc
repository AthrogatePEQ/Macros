#EVENT ReagentCheck "<#1#> ReagentCheck#*#"
SUB EVENT_ReagentCheck(string line, ChatSender)
/if (${Debug} || ${Debug_Reagents}) /echo |- EVENT_ReagentCheck ==>

/declare abilityName string local
/declare minEnd int local
/declare i int local
/declare minCount int local
/declare reagentCount int local
/declare reagent string local
/declare itemSummoner string local

/if ( ${MyReagents.Size} > 0 ) {

	/for i 1 to ${MyReagents.Size}
		/varset abilityName ${MyReagents[${i}].Arg[1,/]}
    /if (${Debug} || ${Debug_Reagents}) /echo ${abilityName}: ${MyReagents[${i}]}

		/if ( ${Me.CombatAbility[${abilityName}]} ) {

			| Check for MinEnd
			/if (${MyReagents[${i}].Find[/MinEnd|]}) {
				/call argueString MinEnd "${MyReagents[${i}]}"
				/varset minEnd ${Macro.Return}
			} else {
				/varset minEnd 0
			}

			| Check for Min
			/if ( ${MyReagents[${i}].Find[/Min|]} ) {
				/call argueString Min "${MyReagents[${i}]}"
				/varset minCount ${Macro.Return}
			}
			/if (${Debug} || ${Debug_Reagents}) /echo Count..: ${minCount}

			| Check for Reagent
			/if ( ${MyReagents[${i}].Find[/Reagent|]} ) {
				/call argueString Reagent "${MyReagents[${i}]}"
				/varset reagent ${Macro.Return}
			}
			|/echo Reagent: ${reagent}


			/call EVENT_Makeaxes "" ${minCount} "${abilityName}" "${reagent}"
		} else /if ( ${MyReagents[${i}].Find[/Buy]} ) {

			/call argueString Min "${MyReagents[${i}]}"
			/varset reagentCount ${Macro.Return}

			/if ( ${Math.Calc[${FindItemCount[=${abilityName}]} + 1].Int} < ${reagentCount} ) {
				/bc Buy more ${abilityName} only ${Math.Calc[${FindItemCount[=${abilityName}]} + 1].Int} left.
			}

		} else /if ( ${MyReagents[${i}].Find[/Summon]} ) {
			/varset minCount 0

			/call argueString Summon "${MyReagents[${i}]}"
			/varset itemSummoner ${Macro.Return}
			|/echo ${itemSummoner}

			/if ( ${MyReagents[${i}].Find[/Min|]} ) {
				/call argueString Min "${MyReagents[${i}]}"
				/varset minCount ${Macro.Return}
				|/echo minCount: ${minCount}
			}

      :SummonMore
			/echo I Have: ${Math.Calc[${FindItemCount[=${abilityName}]} + 1].Int} of ${minCount}
			/if ( ${Math.Calc[${FindItemCount[=${abilityName}]} + 1].Int} < ${minCount} && ${Me.FreeInventory} ) {
				/casting "${itemSummoner}" -targetId|${Me.ID}
				/delay 6s !${Me.Casting.ID}
				/autoinventory
				/delay 1s !${Cursor.ID}
				/goto :SummonMore
			}
		}
	/next i
}
/if (${Debug} || ${Debug_Reagents}) /echo <== EVENT_ReagentCheck -|

/RETURN

#EVENT MakeAxes "[MQ2] MakeAxes #1# #2#"
#EVENT MakeAxes "<#*#> MakeAxes #1# #2#"
SUB EVENT_Makeaxes(string derp, int axeCount, string axeName, string reagentName)

/declare numAxes int local

/if (${Debug} || ${Debug_Reagents}) /echo main |- EVENT_Makeaxes ==>

|/echo axeName.: ${axeName}
|/echo axeCount: ${axeCount}
|/echo How many do I have: ${Math.Calc[${FindItemCount[=${axeName}]}].Int}

/varset numAxes ${Math.Calc[${FindItemCount[=${axeName}]}].Int}

/if (!${Me.FreeInventory}) {
	/echo FULL INVENTORY
	/return
} else {
	/if ( ${Me.CombatAbility[${axeName}]} ) {
		/if (${Debug} || ${Debug_Reagents}) /echo ${axeName}: ${Math.Calc[${FindItemCount[=${axeName}]}].Int} of ${axeCount}
		:MakeMore
		/if ( ${Math.Calc[${FindItemCount[=${axeName}]} + 1].Int} < ${axeCount} ) {

			/if (!${Me.FreeInventory}) {
				/echo FULL INVENTORY
				/return
			}
			/if (!${Me.CombatAbilityReady[${axeName}]}) {
				/delay 1s
				/if (${Debug} || ${Debug_Reagents}) /echo ${axeName} not ready.
			} else {
				/doability "${axeName}"
				/delay 4s
				/autoinv

				|/doevents Missing_Components

				|/if ( ${Macro.Return.Equal[NOREAGENTS]} ) {
				|	/echo DERP: No Axes summoned, probably missing components.
				|	/return
				|}

				/if ( ${Math.Calc[${FindItemCount[=${axeName}]}].Int} == ${numAxes} ) {
					/echo No Axes summoned, probably missing components: ${reagentName}
					/return
				}
				/varset numAxes ${Math.Calc[${FindItemCount[=${axeName}]}].Int}
			}
			/delay 5s
			/echo ${axeName}: ${Math.Calc[${FindItemCount[=${axeName}]}].Int} of ${axeCount}
			/goto :MakeMore
		}

	}
}
/if (${Debug} || ${Debug_Reagents}) /echo main <== EVENT_Makeaxes -|
/RETURN

Sub Reagents_Background_Events
  /doevents ReagentCheck
/return

Sub Reagents_Aliases
/return

Sub Reagents_Setup
  /call #IniToArray "${Character_Ini},Reageant Check,Reageant#" MyReagents

/return

Sub Reagents_CharacterSettings
/return

Sub Reagents_MacroSettings
/return