|--------------------------------------------------------------------------------------------|
|- e3_Tanking.inc v2.0 ( originally designed to work with e3.mac )						              -|
|- Written by Athrogate <Fusion> of PEQ																                      -|
|- Adapted from Shendo's advbot to work with e3 slightly different but inspired by Shendo   -|
|--------------------------------------------------------------------------------------------|

#Event ToggleOfftank "<#1#> Toggle Offtank #2#"
#Event ToggleOfftank "#1# tells you, 'Offtank #2#'"

Sub AutoengageFunction
    /if (${DisableAutoengage}==0) {
        /if (${PauseAutoengage}==0) {
            /varset PauseAutoengage 1
            /if (${Reply}==1 && !${SenderName.Equal[nobody]}) /docommand /${If[${ReplyChannel.Find[ @]}>0,${ReplyChannel.Mid[1,${Math.Calc[${ReplyChannel.Find[ @]}-1]}]} ${SenderName},${ReplyChannel}]} I will NOT auto engage targets.
            /echo -[ Autoengage Paused. ]-
        } else {
            /varset PauseAutoengage 0
            /if (${Reply}==1 && !${SenderName.Equal[nobody]}) /docommand /${If[${ReplyChannel.Find[ @]}>0,${ReplyChannel.Mid[1,${Math.Calc[${ReplyChannel.Find[ @]}-1]}]} ${SenderName},${ReplyChannel}]} I will auto engage targets.
            /echo -[ Autoengage Unpaused. ]-
        }
    } else {
        /varset PauseAutoengage 1
    }
/return

SUB EVENT_ToggleOfftank(line, ChatSender, buffSetting)
/if (${Debug} || ${Debug_OffTank}) /echo offtank |- EVENT_ToggleOfftank ==>

  /if ( ${OffTank} ) {
    /if (! ${Defined[otFlag]} ) /declare otFlag boolean local FALSE

    /declare mystring string local ${buffSetting.Arg[1, ]}
    /call #verifyEvent "${ChatSender}" "${line}"
    /if (!${Macro.Return}) /return

    /if (${mystring.Equal[on]}) {
      /varset PauseOfftank 0
      /varset otFlag TRUE
    } else /if (${mystring.Equal[off]}) {
      /varset PauseOfftank 1
      /varset otFlag FALSE
    }

    /docommand ${ChatToggle} Auto Off Tanking [${If[${otFlag},ON,OFF]}]
  }
/if (${Debug} || ${Debug_OffTank}) /echo offtank <== EVENT_ToggleOfftank -|
/RETURN

Sub OffTanking_Setup
  /declare DisableOfftank int outer
  /declare PauseOfftank int outer 1
  /declare OfftankDelay int outer
  /declare SenderName string outer nobody
  /declare DisableAutoengage int outer
  /declare PauseAutoengage int outer
  /declare AutoTargetID int outer
  /declare OffTank bool outer
  /declare PullerName string outer

  /declare Debug_Offtank int outer 0

  /if (!${Bool[${${IniMode}[${Character_Ini},Tanking,Offtank (On/Off)]}]}) /call OffTanking_CharacterSettings
  /if (${Bool[${${IniMode}[${Character_Ini},Tanking,Offtank (On/Off)]}]}) /call #IniToVar "${Character_Ini},Tanking,Offtank (On/Off)" OffTank bool outer
  /if (${${IniMode}[${Character_Ini},Tanking,TauntAbility].Length}) /call #IniToArray "${Character_Ini},Tanking,TauntAbility#" TankingAbilities

/return

Sub check_OffTanking
  /if (${Debug} || ${Debug_OffTank}) /echo offtank |- OfftankCheck ==>
  /if (!${OffTank} || ${PauseOfftank}==1 ) /return
  /call OfftankAction
/return

Sub OfftankAction
  /declare npc int local
  /declare npca int local
  /declare pc int local
  /declare a int local
  /declare i int local
  /declare g int local
  /declare p int local
  /declare r int local
  /declare range float local
  /declare inrange int local
  /declare npctarget int local
  /declare notidlist string local
  /declare backquote_loc int local

  /varset npc 0
  /varset i 0
  :GetNextNPC
  /doevents BackOff

  /varcalc i ${i}+1
  /varset npc ${NearestSpawn[${i},npc radius 50 zradius 25 los noalert ${AlertList}].ID}
  /if ( ${npc} && ${npc} != 0 && ${Select[${Spawn[id ${npc}].Type},${AcceptableTargetTypes}]}) {
    /varset backquote_loc ${Spawn[id ${npc}].CleanName.Find[`s pet]}
    /if (${backquote_loc} > 0) {
      /if (${Spawn[pc ${Spawn[id ${npc}].CleanName.Left[${Math.Calc[${backquote_loc}-1].Int}]}].ID}) /goto :GetNextNPC
    }
    /squelch /target id ${npc}
    /delay 1s ${Target.ID}==${npc}
    /varcalc range ${Spawn[${npc}].MaxRange} * 1.1
    /varset inrange 0
    /varset p 0
    :GetNextPC
    /varcalc p ${p}+1
    /varset pc ${Spawn[${npc}].NearestSpawn[${p}, pc radius ${range} zradius 25].ID}

    /if (${pc}) {
      /if (${Spawn[${pc}].Guild.Equal[${Me.Guild}]} || ${Raid.Member[${Spawn[${pc}].CleanName}].Name.Length}>0) {
        /varset inrange ${pc}
        /goto :InRange
      }
      /goto :GetNextPC
    }

    :InRange
    /if (${inrange}) {
      /squelch /target id ${npc}
      /delay 1s ${Target.ID}==${npc}
      /if ( ${Me.ID}!=${Me.TargetOfTarget.ID} && ${Target.ID} > 0 ) {
        /if ( ${Select[${Me.TargetOfTarget.Class.ShortName},WAR,PAL,SHD]} < 1 ) {
          /call EngageTarget
          /goto :DoneSearching
        }
      }
    }
    /goto :GetNextNPC
  }
  /if (${PauseOfftank}==1) /squelch /target clear

  /goto :NoTargetFound
  :DoneSearching
  /if (${Me.Combat}) {
    /if (${Me.Sitting}) /stand

    /if (${PauseOfftank}==0) {

      /if (${Me.ID}!=${Me.TargetOfTarget.ID} && ${Target.ID} > 0 ) {
       /bc "AUTO Attacking ${Target.CleanName} ID: ${Target.ID} (near: ${Spawn[${inrange}].CleanName}, target: ${Me.TargetOfTarget}."
      }

      :GetNPCAggro
      /call AggroAbilities
      /goto :GetNextNPC
    }

  }
  :NoTargetFound
  /if (${Debug} || ${Debug_OffTank}) /echo offtank <== OfftankCheck -|

/return

Sub EngageTarget
  /if (${Debug} || ${Debug_OffTank}) /echo offtank |- EngageTarget ==>
  /netfollow off
  /stick off

  /if ( ${Target.ID} != 0) {

    /stick 5 front uw
    /attack on

    /delay 1s (${Target.Distance} <= ${Spawn[${Target.ID}].MaxRangeTo})
  }

/return

SUB AggroAbilities
/if (${Debug} || ${Debug_OffTank}) /echo offtank |- AggroAbilities ==>
	| Check Target distance.
	/if (${Target.Distance} > ${Spawn[${Target.ID}].MaxRangeTo}) {
		/if (${Debug} || ${Debug_OffTank}) /echo offtank I am too far away from [${Spawn[id ${AssistTarget}].CleanName}] to use abilities.
	} else {

		/declare abilityName string local
		/declare minEnd int local
		/declare j int local
    :DoAggroAbilities
    /doevents Backoff
		| For 1 to the size of MyAbilities array, use each ability.
		/for j 1 to ${TankingAbilities.Size}
		  /if (${Debug} || ${Debug_OffTank}) /echo offtank Checking "${TankingAbilities[${j}]}"
			/varset abilityName ${TankingAbilities[${j}].Arg[1,/]}

      | Use ability.
      /if (${Me.Ability[${abilityName}]}) {

        /if (!${Me.AbilityReady[${abilityName}]}) {
          /if (${Debug} || ${Debug_OffTank}) /echo offtank Ability [${abilityName}] is not ready.
        } else {
          | Use Taunt.
          /if (${abilityName.Equal[Taunt]}) {
            |/call Attempt_Taunt
            /if (${Select[${Me.TargetOfTarget},${Me.CleanName}]}) {
              /echo I currently have aggro on [${Target.CleanName}], skipping taunt.
            } else {
              /doability Taunt
            }

          | Use Bash.
          } else /if (${abilityName.Equal[Bash]}) {
            |/call Attempt_Bash
            /if (!${Select[${Me.Inventory[Offhand].Type},Shield]} && !${Me.AltAbility[2 Hand Bash]}) {
              /echo You must equip a shield, or train in the alternate ability "2 Hand Bash" to use the ability [Bash].
            } else {
              /doability Bash
            }

          | Use a misc Ability.
          } else {
            | Misc.
            /doability "${abilityName}"
          }
        }

      | Use Combat Ability
      } else /if (${Me.CombatAbility[${abilityName}]}) {
        /if (${Me.CombatAbilityReady[${abilityName}]}) {
          /doability "${abilityName}"
        }

      | Use alternate ability
      } else /if (${Me.AltAbility[${abilityName}]}) {
        /if (${Me.AltAbilityReady[${abilityName}]}) {
          |/call e3_Cast ${Target.ID} "${abilityName}" alt
          /casting "${abilityName}" alt -maxtries|3
        }
      } else /if (${Me.Gem[${abilityName}]} && ${Me.SpellReady[${abilityName}]} ) {
        /casting "${abilityName}" -maxtries|3
        /delay 4s ${Cast.Ready}
      }

			/if (${Me.ID}==${Me.TargetOfTarget.ID} || ${Target.ID} == 0 || ${Select[${Me.TargetOfTarget.Class.ShortName},WAR,PAL,SHD]} > 0 || ${Target.Dead} ) /return
			/delay 5
		/next j
		/if ( ${Select[${Me.TargetOfTarget.Class.ShortName},WAR,PAL,SHD]} < 1 ) /goto :DoAggroAbilities
	}

/if (${Debug} || ${Debug_OffTank}) /echo offtank <== AggroAbilities -|
/RETURN

Sub OffTanking_Aliases
/if (${Debug} || ${Debug_OffTank}) /echo offtank |- tanking_Aliases ==>

  /squelch /alias /offtank /bc Toggle Offtank
  /squelch /alias /ot /bc Toggle Offtank

/if (${Debug} || ${Debug_OffTank}) /echo offtank <== tanking_Aliases -|
/return

Sub OffTanking_CharacterSettings
/if (${Debug} || ${Debug_OffTank}) /echo offtank |- Tanking_CharacterSettings ==>
  /if ( ${Select[${Me.Class.ShortName},WAR,PAL,SHD]} ) {
    /call WriteToIni "${Character_Ini},Tanking,Offtank (On/Off)" "Off"
    /call WriteToIni "${Character_Ini},Tanking,TauntAbility" "Taunt"
  }
/if (${Debug} || ${Debug_OffTank}) /echo offtank <== Tanking_CharacterSettings -|
/return

Sub OffTanking_MacroSettings
/return

Sub OffTanking_Background_Events
  /doevent ToggleOfftank
/return

Sub OffTanking_Setup

/return