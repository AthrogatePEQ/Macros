|--------------------------------------------------------------------------------|
|- e3_Classes_Necromancer.inc v4.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ													-|
|--------------------------------------------------------------------------------|
#Event ToggleManaDump "<#1#> Mana Dump #2#"
#Event ToggleManaDump "#1# tells you, 'Mana Dump #2#'"
SUB Event_ToggleManaDump(line, ChatSender, buffSetting)
/if (${Debug}) /echo offtank |- ToggleManaDump ==>

	/if (! ${Defined[manaDump]} ) /declare manaDump boolean local FALSE

	/declare mystring string local ${buffSetting.Arg[1, ]}
	/call #verifyEvent "${ChatSender}" "${line}"
	/if (!${Macro.Return}) /return

	/if (${mystring.Equal[on]}) {
		/varset manaDump TRUE
	} else /if (${mystring.Equal[off]}) {
		/varset manaDump FALSE
	}

	/docommand ${ChatToggle} Auto Mana Dump [${If[${manaDump},ON,OFF]}]

/if (${Debug}) /echo offtank <== ToggleManaDump -|
/RETURN

SUB check_Lich
/if (${Debug}) /echo |- check_castLich ==>
	/if (${Defined[lichSpell]} && !${Me.Buff[${lichSpell}].ID} && ${Me.PctMana} < 90 && ${Spell[${lichSpell}].Stacks} && ${Me.PctHPs} > 80) /casting "${lichSpell}"
	/delay 5s !${Me.Casting.ID}
  /if (${Me.Buff[${lichSpell}].ID} && ${Me.PctHPs} < 20 && ${Me.CombatState.NotEqual[COMBAT]})  /removebuff ${lichSpell}
/if (${Debug}) /echo <== check_castLich -|
/RETURN

SUB check_manaDump
/if (${Debug}) /echo |- check_manaDump ==>
	/if (${manaDump} && !${medBreak} && ${BuffCheck} && ${UserBuffCheck} ) {
		/declare manaDump_Target string local
		/declare manaDumpSpellIndex int local 0
    /declare i int local
    /declare s int local

    /for s 1 to ${manaDump_Spells2D.Size[1]}
      /call check_Ready "manaDump_Spells2D" ${s}
      /if (${Macro.Return}) {
      	/call check_Mana "manaDump_Spells2D" ${s}
				/if (${Macro.Return}) {
      		/varset manaDumpSpellIndex ${s}
				}
			}
    /if (${manaDumpSpellIndex}==0) /next s

    /if (${manaDumpSpellIndex} > 0 ) {
      /for i 1 to ${whoto_manaDump.Size}
        /call check_Distance ${NetBots[${whoto_manaDump[${i}].Arg[1,/]}].ID} ${manaDump_Spells2D[${manaDumpSpellIndex},${iMyRange}]}
        /if (${Macro.Return}) {
        	/if (${NetBots[${whoto_manaDump[${i}].Arg[1,/]}].PctMana} <= ${whoto_manaDump[${i}].Arg[2,/]}) {
          	/call e3_Cast ${NetBots[${whoto_manaDump[${i}].Arg[1,/]}].ID} "manaDump_Spells2D" ${manaDumpSpellIndex}
          }
				}
			/if (!${ActionTaken}) /next i
    }
  }
/if (${Debug}) /echo <== check_manaDump -|
/RETURN

Sub check_NECAggroControl
/if (${Debug}) /echo |- MNK_AggroControl ==>
|only use evade over 80% aggro, and only if im using a disc (burning)
	/if (${Me.PctAggro} > 60 && ${Me.AltAbilityReady[Death Peace]}) {
		/attack off
		/delay 1s !${Me.Combat}
		/casting "Death Peace"
		/delay 5 ${Me.Feigning}
		/stand
		/attack on
	}
/if (${Debug}) /echo <== MNK_AggroControl -|
/return

Sub NEC_Setup
/if (${Debug}) /echo |- NEC_Setup ==>
	/if (${Ini[${Character_Ini},${Me.Class},Lich Spell].Length}) /call IniToVar "${Character_Ini},${Me.Class},Lich Spell" lichSpell string outer
	/call #IniToArray "${Character_Ini},${Me.Class},LifeTap#" lifeTaps
	/if (${lifeTaps.Size}) /call BuildSpellArray "lifeTaps"	"lifeTaps2D"
	/if (${Ini[${Character_Ini},${Me.Class},Mana Dump (On/Off)].Length}) /call IniToVar "${Character_Ini},${Me.Class},Mana Dump (On/Off)" manaDump bool outer
  /call #IniToArray "${Character_Ini},${Me.Class},Who to Mana Dump#" whoto_manaDump
	/call #IniToArray "${Character_Ini},${Me.Class},Mana Dump#" manaDump_Spells
  /if (${manaDump_Spells.Size}) /call BuildSpellArray "manaDump_Spells"	"manaDump_Spells2D"
/if (${Debug}) /echo <== NEC_Setup -|
/return

SUB NEC_MacroSettings
/if (${Debug}) /echo |- NEC_MacroSettings ==>
	/call WriteToIni "${advSettings_Ini},NEC Functions,NEC Function" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},NEC Functions,NEC Function" "check_Lich" 1
	/call WriteToIni "${advSettings_Ini},NEC Functions,NEC Function" "check_manaDump" 1
	/call WriteToIni "${advSettings_Ini},NEC Functions,NEC Function" "check_Debuffs" 1
	/call WriteToIni "${advSettings_Ini},NEC Functions,NEC Function" "check_DoTs" 1
	/call WriteToIni "${advSettings_Ini},NEC Functions,NEC Function" "check_lifeTap" 1
	/call WriteToIni "${advSettings_Ini},NEC Functions,NEC Function" "check_Nukes" 1
	/call WriteToIni "${advSettings_Ini},NEC Functions,NEC Function" "check_Buffs" 1
	/call WriteToIni "${advSettings_Ini},NEC Functions,NEC Function" "check_Pets" 1
/if (${Debug}) /echo <== NEC_MacroSettings -|
/RETURN

SUB NEC_CharacterSettings
/if (${Debug}) /echo |- NEC_CharacterSettings ==>
	/call WriteToIni "${Character_Ini},${Me.Class},Lich Spell"
	/call WriteToIni "${Character_Ini},${Me.Class},LifeTap"
	/call WriteToIni "${Character_Ini},${Me.Class},Mana Dump (On/Off)"
  |/call WriteToIni "${Character_Ini},${Me.Class},Mana Dump Engage Pct" 70
	/call WriteToIni "${Character_Ini},${Me.Class},Who to Mana Dump"
	/call WriteToIni "${Character_Ini},${Me.Class},Mana Dump"
/if (${Debug}) /echo <== NEC_CharacterSettings -|
/RETURN

Sub NEC_Aliases
/if (${Debug}) /echo |- NEC_Aliases ==>
	/noparse /squelch /alias /manadump /bc Mana dump
/if (${Debug}) /echo <== NEC_Aliases -|
/return

Sub NEC_Background_Events
	/doevent ToggleManaDump
/return