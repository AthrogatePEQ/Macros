|
| FocusFinder.inc r2
|   Finds equipped focuses
|
|   /alias /findfocus /bc FindFocus
|   /alias /listfocustypes /echo List Focus Types
|
|   Syntax for /findfocus (in brackets optional):
|     /findfocus [missing] alias [affinity] [inclusion args]
|
|     missing: optional. Add to only show if the focus isn't found
|     alias: which type of focus to search for (EH, ER, SH etc) 
|     affinity: optional. examples: fire, beneficial. will only search for focuses of that specific type
|
|   Example usage:
|     I want to know which of my healers are missing enhanced healing:
|       /findfocus missing EH /only|healers
|     I want to know which healers have enhanced healing, and what %:
|       /findfocus EH /only|healers
|     I want to know if my wizards have a fire damage focus:
|       /findfocus ID fire /only|wiz
|     I want a list of my wizard's damage focuses (will show one for each damage type):
|       /findfocus ID /only|wiz
|     I want to know if my wizards have detrimental spell haste:
|       /findfocus SH detrimental /only|wiz
|     I want to know if my wizards have either type of spell haste:
|       /findfocus SH /only|wiz
|     I want to know if my rogues are missing a crit focus:
|       /findfocus missing crit /only|rog

#Event ListFocusTypes "[#*#] List#*#Focus#*#Types"
Sub Event_ListFocusTypes
/echo Event_ListFocusTypes ${focusfinder_ini}
  /declare i int local 0
  /declare currentFocusType string local
  /for i 1 to ${Ini[${focusfinder_ini},Aliases].Count[|]}
    /varset currentFocusType ${Ini[${focusfinder_ini},Aliases].Arg[${i},|]}
    /if (${currentFocusType.Length} && !${Select[${currentFocusType},NULL]}) {
      /echo ${currentFocusType}: ${Ini[${focusfinder_ini},Aliases,${currentFocusType}].Arg[1,|]}
    }
  /next i
/return

#Event FindFocus "<#1#> FindFocus#2#"
Sub Event_FindFocus(line, chatSender, args)
  /if (${args.Length} == 0) {
    /if (${chatSender.Equal[${Me.Name}]}) /call Event_ListFocusTypes
    /return
  }
  /declare i int local 0
  /call #CreateVars string outer ItemEffectsReturn Aug1 Aug2 Aug3 Aug4 Aug5 FocusReturn FoundFocus ItemLink DisplayType
  
  /declare currentID int local
  /declare currentLoc string local
  /declare highestFoundID int local 0
  /declare highestFoundValue int local
  /declare locationOfHighest string local
  /declare spellID int local 0
  /declare findMissing int local 0
  /declare typeToFind string local
  /declare infoOfSpellType string local
  /declare foundCount int local 0
  /declare specifiedSpellAffinity string local
  /declare FoundString string local
  /declare alreadyFound string local
  
  /call #verifyEvent "${chatSender}" "${line}"
  /if (!${Macro.Return}) /return
  /if (${args.Arg[1, ].Equal[missing]}) {
    /varset findMissing 1
    /varset typeToFind ${args.Arg[2, ]}
  } else {
    /varset typeToFind ${args.Arg[1, ]}
  }
  /if (${args.Arg[2, ].Find[bene]} || ${args.Arg[3, ].Find[bene]}) {
    /varset specifiedSpellAffinity Beneficial
  }
  /if (${args.Arg[2, ].Find[detri]} || ${args.Arg[3, ].Find[detri]}) {
    /varset specifiedSpellAffinity Detrimental
  }
  
  /if (${Select[${args.Arg[2, ]},cold,fire,magic,poison,disease,all]}) {
    /varset specifiedSpellAffinity ${args.Arg[2, ]}
  }
  /if (${Select[${args.Arg[3, ]},cold,fire,magic,poison,disease,all]}) {
    /varset specifiedSpellAffinity ${args.Arg[3, ]}
  }
  
  
  /varset infoOfSpellType ${Ini[${focusfinder_ini},Aliases,${typeToFind}]}
  /if (${specifiedSpellAffinity.Length}) {
    /varset infoOfSpellType ${infoOfSpellType.Arg[1,|]}|${infoOfSpellType.Arg[2,|]}|${specifiedSpellAffinity}
  }
  /varset DisplayType ${infoOfSpellType.Arg[2,|]}
  
  /declare ItemEffectsString string local
  /for i 0 to 20
    /call GetItemEffects "InvSlot[${i}].Item" "${InvSlot[${i}].Name}" ItemEffectsReturn
    /if (${ItemEffectsReturn.Length}) /varset ItemEffectsString ${ItemEffectsString}${If[${ItemEffectsString.Length},|,]}${ItemEffectsReturn}
  /next i
  
  /if (${Ini[${focusfinder_ini},Items,${typeToFind}].Length}) {
    /declare externalItems string local ${Ini[${focusfinder_ini},Items,${typeToFind}]}
    /for i 1 to ${Math.Calc[${externalItems.Count[|]}+1]}
      /if (${FindItemCount[=${externalItems.Arg[${i},|]}]}) {
        /call GetItemEffects "FindItem[=${externalItems.Arg[${i},|]}]" "${FindItem[=${externalItems.Arg[${i},|]}].InvSlot}" ItemEffectsReturn
        /if (${ItemEffectsReturn.Length}) /varset ItemEffectsString ${ItemEffectsString}${If[${ItemEffectsString.Length},|,]}${ItemEffectsReturn}
      }
    /next i
  }
  
  |/echo ${ItemEffectsString}
  /for i 3 to ${Math.Calc[${infoOfSpellType.Count[|]}+1]}
    |/echo Finding Focus: ${infoOfSpellType.Arg[1,|]} (${infoOfSpellType.Arg[${i},|]})
    /call FindFocusOfType "${ItemEffectsString}" "${typeToFind}" "${infoOfSpellType.Arg[${i},|]}" FoundFocus
    /if (!${findMissing}) {
      /if (${FoundFocus.Length} && !${FoundString.Find[${FoundFocus}]}) {
        /if (${FoundString.Length}) {
          /varset FoundString ${FoundString}|${FoundFocus}
        } else {
          /varset FoundString ${FoundFocus}
        }
      }
    } else {
      /if (${FoundFocus.Length}) /varcalc foundCount ${foundCount}+1
    }
  /next i
  /if (${findMissing} && !${foundCount}) {
    /if (${chatSender.Equal[${Me.Name}]}) {
      /echo Missing ${infoOfSpellType.Arg[1,|]}${If[${specifiedSpellAffinity.Length}, (${specifiedSpellAffinity}),]}
    } else {
      /tell ${chatSender} Missing ${infoOfSpellType.Arg[1,|]}${If[${specifiedSpellAffinity.Length}, (${specifiedSpellAffinity}),]}
    }
  } else {
    /if (${FoundString.Length}) {
      /if (${chatSender.Equal[${Me.Name}]}) {
        /echo ${FoundString}
      } else {
        /tell ${chatSender} ${FoundString}
      }
    } 
  }
/return

Sub FindFocusOfType(ItemEffects, typeToFind, spellAffinity, destVar)
  /declare i int local
  /declare currentID int local
  /declare currentLoc string local
  /declare highestFoundID int local 0
  /declare highestFoundValue int local 0
  /declare locationOfHighest string local
  /for i 1 to ${Math.Calc[${ItemEffects.Count[|]}+1]}
    /varset currentID ${ItemEffects.Arg[${i},|].Arg[1,@]}
    /varset currentLoc ${ItemEffects.Arg[${i},|].Arg[2,@]}
    /call CheckFocusType ${currentID} "${typeToFind}" ${spellAffinity} FocusReturn
    /if (${FocusReturn.Length} && ${FocusReturn} > ${highestFoundValue}) {
      /varset highestFoundValue ${FocusReturn} 
      /varset highestFoundID ${currentID}
      /varset locationOfHighest ${currentLoc}
    }
  /next i
  
  /if (${highestFoundID}) {
    /if (!${locationOfHighest.Find[->]}) {
      /call #GetItemLink "${InvSlot[${locationOfHighest}].Item.Name}"
      /varset ${destVar} ${Spell[${highestFoundID}]}: ${If[${DisplayType.Equal[Exists]},Exists,${highestFoundValue}]}${If[${DisplayType.Equal[Percent]},%,]} on ${Macro.Return}
      |${If[${InvSlot[${locationOfHighest}].ID}>20, (Unequipped),]}
    } else {
      /declare itemPart string local ${locationOfHighest.Arg[1,->]}
      /declare augPart string local ${locationOfHighest.Arg[2,->].Right[-4]}
      |/echo ${locationOfHighest}
      |/echo itempart: ${itemPart}
      |/echo augpart: ${augPart}
      /call #GetItemLink "${InvSlot[${itemPart}].Item.Item[${augPart}].Name}"
      /declare augLink ${Macro.Return}
      /call #GetItemLink "${InvSlot[${itemPart}].Item.Name}"
      /declare itemLink ${Macro.Return}
      /varset ${destVar} ${Spell[${highestFoundID}]}: ${If[${DisplayType.Equal[Exists]},Exists,${highestFoundValue}]}${If[${DisplayType.Equal[Percent]},%,]} on ${augLink} in ${itemLink}
      |${If[${InvSlot[${itemPart}].ID}>20, (Unequipped),]}
    }
  } else {
    /varset ${destVar}
  }
/return 

Sub CheckFocusType(SpellID, CheckForType, spellAffinity, destVar)
  /declare SpellInfo string local
  /varset SpellInfo ${Ini[${focusfinder_ini},Spells,ID${SpellID},]}
  /if (${SpellInfo.Length}) {
    /if (${SpellInfo.Arg[1,|].Equal[${CheckForType}]} && (${SpellInfo.Arg[4,|].Equal[${spellAffinity}]} || ${SpellInfo.Arg[4,|].Equal[Both]} || ${SpellInfo.Arg[4,|].Equal[All]})) {
      /varset ${destVar} ${SpellInfo.Arg[3,|]}
      /return
    }
  }
  /varset ${destVar}
/return

Sub GetItemEffects(ItemRef, LocRef, destVar)
  /declare i int local
  /declare effectList string local
  
  /varset ${destVar}
  /if (${${ItemRef}.EffectCount}) {
    /for i 1 to ${${ItemRef}.EffectCount}
      /varset effectList ${effectList}|${${ItemRef}.Effects[${i}].ID}@${LocRef}
    /next i
  }
  /if (${${ItemRef}.Augs}) {
    /for i 1 to ${${ItemRef}.Augs}
      /if (${Bool[${${ItemRef}.Item[${i}]}]}) {
        /call GetItemEffects "${ItemRef}.Item[${i}]" "${LocRef}->Aug${i}" Aug${i}
        /if (${Aug${i}.Length}) {
          /varset effectList ${effectList}|${Aug${i}}
        }
      }
    /next i
  }
  /varset ${destVar} ${effectList}
  /if (${${destVar}.Left[1].Equal[|]}) /varset ${destVar} ${${destVar}.Right[-1]}
/return

Sub FocusFinder_Setup
  /declare focusfinder_ini string outer e3 Macro Inis\FocusFinder.ini
/return

SUB FocusFinder_CharacterSettings
/RETURN

Sub FocusFinder_MacroSettings
/return

Sub FocusFinder_aliases
  /alias /findfocus /bc FindFocus
  /alias /listfocustypes /echo List Focus Types
/return

Sub FocusFinder_Background_Events
	/doevents FindFocus
	/doevents ListFocusTypes
/return
