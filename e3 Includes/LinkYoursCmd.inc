|
| LinkYoursCmd.inc r4
|   Adds an eqbc chat command to show bots current equipment in a given slot.
|   Responses are item links sent via tell.
|
|   Run this command to create an alias:
|     /alias /linkyours /bc LinkYours
|
|   Syntax for /linkyours is:
|     /linkyours slot filter
|       filter - e3 style filter using /only, /not etc
|   
|   Example: I want to know what my healers are wearing in their finger slots. These will all do the same thing:
|     /linkyours ring /only|priests
|     /linkyours ring /only|healers
|     /linkyours ring /only|CLR DRU SHM
|
|   Example: I want to know what everyone has in their charm slot:
|     /linkyours charm
|
| Requires
|#include vyslib.inc
|#include vyslib_items.inc

#Event LinkYoursCmd "<#1#> LinkYours #2#"
#Event ReportEmptySlots "<#*#> ReportEmptySlots"
Sub LinkYoursCMD_Background_Events
  /doevents LinkYoursCMD
  /doevents ReportEmptySlots
/return

Sub LinkYoursCMD_Aliases
  /alias /linkyours /bc LinkYours
  /alias /ly /bc LinkYours
/return

Sub LinkYoursCMD_Setup
/return

Sub LinkYoursCMD_CharacterSettings
/return

sub LinkYoursCMD_MacroSettings
/return

Sub Event_LinkYoursCmd(line, chatSender, args)
  /declare LY:TempLink string local
  /declare displayAugs bool local false
  /declare i int local

  /call #CreateVars string outer LY:Return LY:Temp LY:Slots LY:Slot AugsList TruthArgs InclusionPart ArgsPart

  /if (${args.Find[||]}) /varset args ${args.Replace[||, or ]}

  /call #SplitInclusionArgs "${args}" ArgsPart InclusionPart "/augs /show /filter /empty"

  /declare LY:ShowFilter int local 1
  /if (${InclusionPart.Find[/filter]}) /varset LY:ShowFilter 0

  /declare LY:OnlyEmpty int local 0
  /if (${InclusionPart.Find[/empty]}) /varset LY:OnlyEmpty 1

  /if (${InclusionPart.Length}) {
    /call #verifyEvent "${chatSender}" "${InclusionPart}"
    /if (!${Macro.Return}) /return
  }

  /if (${InclusionPart.Find[/augs]}) {
    /call #RequirePlugin MQ2Format
    /if (${Macro.Return}) {
      /varset displayAugs true
    } else {
      /varset displayAugs false
      /echo Unable to load MQ2Format, will not display augs.
    }
  }

  /if (${InclusionPart.Find[/if|]}) {
    /call #SplitTruthArgs "${InclusionPart}" /if TruthArgs "/augs|/show|/filter|/empty"
  }

  /declare reportedSlots int local 0
  /declare filteredSlots int local 0

  /declare LY:Response string local

  /call LY:GetSlots "${ArgsPart}" LY:Slots
  /if (${LY:Slots.Count[|]} > 4) /varset LY:ShowFilter 0

  /if (${LY:Slots.Length}) {
    /for i 1 to ${LY:Slots.Count[|]}
      /varset LY:Slot ${LY:Slots.Arg[${i},|]}
      /if (${InvSlot[${LY:Slot}].Item.ID}) {
        /if (${LY:OnlyEmpty}) /goto :NextI
        /if (${TruthArgs.Length}) {
          /call ProcessItemTruth "${LY:Slot}" "${TruthArgs} "
          /if (!${Macro.Return}) {
            /varcalc filteredSlots ${filteredSlots} + 1
            /if (${LY:ShowFilter}) /varset LY:Response ${LY:Response}Filtered|
            /goto :NextI
          }
        }
        /varcalc reportedSlots ${reportedSlots} + 1

        /varset LY:Return ${InvSlot[${LY:Slot}].Item.ItemLink}

        /varset LY:Response ${LY:Response}${LY:Return}
        /if (${InclusionPart.Find[/show|]}) {
          /call ProcessShowArgs "${InclusionPart}" "${LY:Slot}"
          /if (${Bool[${Macro.Return}]}) {
            /varset LY:Response ${LY:Response} (${Macro.Return})|
          } else {
            /varset LY:Response ${LY:Response}|
          }
        } else {
          /varset LY:Response ${LY:Response}|
        }
      } else {
        /if (${LY:Slot.Equal[offhand]}) {
          /if (!${InvSlot[mainhand].Item.Type.Find[2H]}) {
            /varcalc reportedSlots ${reportedSlots} + 1
            /varset LY:Response ${LY:Response}Empty${If[${LY:Slots.Count[|]}>=3,[${LY:Slot}],]}|
          }
        } else {
          /varcalc reportedSlots ${reportedSlots} + 1
          /varset LY:Response ${LY:Response}Empty${If[${LY:Slots.Count[|]}>=3,[${LY:Slot}],]}|
        }
      }
      :NextI
    /next i

    /if (!${LY:ShowFilter}) /varset filteredSlots 0
    /if (${filteredSlots} && !${reportedSlots}) /varset LY:Response
    /if (${LY:Response.Length} && ${Math.Calc[${filteredslots} + ${reportedSlots}].Int} > 0) /varset LY:Response ${LY:Response.Left[-1]}
    /if (${LY:Response.Length}) {
      /tell ${chatSender} ${LY:Response}
    }
  }

/return

Sub Event_LinkYoursCmd_deprecated(line, chatSender, args)
  /declare LY:TempLink string local
  /declare displayAugs bool local false
  /declare i int local

  /call #CreateVars string outer LY:Return LY:Temp LY:Slots LY:Slot AugsList TruthArgs InclusionPart ArgsPart

  /if (${args.Find[||]}) /varset args ${args.Replace[||, or ]}

  /call #SplitInclusionArgs "${args}" ArgsPart InclusionPart "/augs /show /filter /empty"

  /declare LY:ShowFilter int local 1
  /if (${InclusionPart.Find[/filter]}) /varset LY:ShowFilter 0

  /declare LY:OnlyEmpty int local 0
  /if (${InclusionPart.Find[/empty]}) /varset LY:OnlyEmpty 1

  /if (${InclusionPart.Length}) {
    /call #verifyEvent "${chatSender}" "${InclusionPart}"
    /if (!${Macro.Return}) /return
  }

  /if (${InclusionPart.Find[/augs]}) {
    /call #RequirePlugin MQ2Format
    /if (${Macro.Return}) {
      /varset displayAugs true
    } else {
      /varset displayAugs false
      /echo Unable to load MQ2Format, will not display augs.
    }
  }

  /if (${InclusionPart.Find[/if|]}) {
    /call #SplitTruthArgs "${InclusionPart}" /if TruthArgs "/augs|/show|/filter|/empty"
  }

  |/if (!${Plugin[MQ2LinkDB].Name.Length}) /plugin mq2linkdb noauto
  |/if (!${Plugin[MQ2LinkDB].Name.Length}) {
  |  /bc I can't load MQ2LinkDB.
  |  /return
  |}

  /declare reportedSlots int local 0
  /declare filteredSlots int local 0

  /declare LY:Response string local

  /call LY:GetSlots "${ArgsPart}" LY:Slots
  /if (${LY:Slots.Count[|]} > 4) /varset LY:ShowFilter 0

  /if (${LY:Slots.Length}) {
    /for i 1 to ${LY:Slots.Count[|]}
      /varset LY:Slot ${LY:Slots.Arg[${i},|]}
      /if (${InvSlot[${LY:Slot}].Item.ID}) {
        /if (${LY:OnlyEmpty}) /goto :NextI
        /if (${TruthArgs.Length}) {
          /call ProcessItemTruth "${LY:Slot}" "${TruthArgs} "
          /if (!${Macro.Return}) {
            /varcalc filteredSlots ${filteredSlots} + 1
            /if (${LY:ShowFilter}) /varset LY:Response ${LY:Response}Filtered|
            /goto :NextI
          }
        }
        /varcalc reportedSlots ${reportedSlots} + 1
        /if (${displayAugs}) {
          /call #GetItemAugIDs ${LY:Slot} AugsList
          /call #BuildItemLink "${InvSlot[${LY:Slot}].Item.Name}" ${AugsList} LY:Return
        } else {
          /call #GetItemLink "${InvSlot[${LY:Slot}].Item.Name}" LY:Return
        }

        /varset LY:Response ${LY:Response}${LY:Return}
        /if (${InclusionPart.Find[/show|]}) {
          /call ProcessShowArgs "${InclusionPart}" "${LY:Slot}"
          /if (${Bool[${Macro.Return}]}) {
            /varset LY:Response ${LY:Response} (${Macro.Return})|
          } else {
            /varset LY:Response ${LY:Response}|
          }
        } else {
          /varset LY:Response ${LY:Response}|
        }
      } else {
        /if (${LY:Slot.Equal[offhand]}) {
          /if (!${InvSlot[mainhand].Item.Type.Find[2H]}) {
            /varcalc reportedSlots ${reportedSlots} + 1
            /varset LY:Response ${LY:Response}Empty${If[${LY:Slots.Count[|]}>=3,[${LY:Slot}],]}|
          }
        } else {
          /varcalc reportedSlots ${reportedSlots} + 1
          /varset LY:Response ${LY:Response}Empty${If[${LY:Slots.Count[|]}>=3,[${LY:Slot}],]}|
        }
      }
      :NextI
    /next i

    /if (!${LY:ShowFilter}) /varset filteredSlots 0
    /if (${filteredSlots} && !${reportedSlots}) /varset LY:Response
    |/if (${LY:Response.Length} && ${Math.Calc[${filteredslots} + ${reportedSlots}].Int} > 0) /varset LY:Response ${LY:Response.Left[-1]}
    /if (${LY:Response.Length}) {
      /tell ${chatSender} ${LY:Response}
    }
  }

/return

Sub LY:GetSlots(ref, DestVar)
  /varset ${DestVar}
  /if (${Select[${ref},charm,leftear,head,face,rightear,neck,shoulder,arms,back,leftwrist,rightwrist,ranged,hands,mainhand,offhand,leftfinger,rightfinger,chest,legs,feet,waist,powersource,ammo]}) {
    /varset ${DestVar} ${ref}|
    /return
  }
  /if (${Select[${ref},all]}) {
    /varset ${DestVar} charm|leftear|head|face|rightear|neck|shoulder|arms|back|leftwrist|rightwrist|ranged|hands|mainhand|offhand|leftfinger|rightfinger|chest|legs|feet|waist|
    /return
  }
  /if (${Select[${ref},ear,ears,earrings,earring]}) {
    /varset ${DestVar} leftear|rightear|
  } else /if (${Select[${ref},wrist,wrists,bracers,bracer]}) {
    /varset ${DestVar} leftwrist|rightwrist|
  } else /if (${Select[${ref},ring,rings,fingers,finger]}) {
    /varset ${DestVar} leftfinger|rightfinger|
  } else /if (${ref.Equal[primary]}) {
    /varset ${DestVar} mainhand|
  } else /if (${ref.Equal[secondary]}) {
    /varset ${DestVar} offhand|
  } else /if (${ref.Equal[range]}) {
    /varset ${DestVar} ranged|
  } else /if (${ref.Equal[belt]}) {
    /varset ${DestVar} waist|
  } else /if (${ref.Equal[shoulders]}) {
    /varset ${DestVar} shoulder|
  }
  /if (${${DestVar}.Length}) /return

  /call #CreateVars string outer ItemLinkReturn

  /call #IsItemLink "${ref}" ItemLinkReturn
  /if (!${ItemLinkReturn}) /return

  /call #RequirePlugin MQ2Format
  /if (!${Macro.Return}) /return

  /call #StripItemLink "${ref}" LY:Return
  /varset ItemLinkReturn
  /call #GetItemLink "${LY:Return}" ItemLinkReturn
  /if (!${ItemLinkReturn.Length}) /return
  /call #ItemLinkToClickable "${ItemLinkReturn}" ItemLinkReturn
  /cleanup
  /delay 50 !${Window[ItemDisplay].Open}
  /notify ChatWindow CW_ChatOutput link ${ItemLinkReturn}
  /delay 50 ${Window[ItemDisplay].Open}


  /varset LY:Temp DisplayItem
  /declare i int local
  /for i 1 to ${${LY:Temp}.Classes}
    /if (${${LY:Temp}.Class[${i}].Name.Equal[${Me.Class.Name}]}) /goto :FoundClass
  /next i
  /cleanup
  /return
  :FoundClass
  /for i 1 to ${${LY:Temp}.WornSlots}
  /varset ${DestVar} ${${DestVar}}${${LY:Temp}.WornSlot[${i}].Name}|
  /next i
  /cleanup

/return

Sub Event_ReportEmptySlots
  /if (!${Defined[LY:EmptySlots]}) /declare LY:EmptySlots string outer
  /call GetEmptySlots LY:EmptySlots
  /if (${LY:EmptySlots.Length}) {
    /bc ${LY:EmptySlots}
  }
/return

Sub GetEmptySlots(destVar)
  /declare iter string local mainhand|charm|head|face|leftear|rightear|neck|shoulder|arms|back|leftwrist|rightwrist|ranged|hands|leftfinger|rightfinger|chest|legs|feet|waist
  /declare i int local 0
  /varset ${destVar}
  /for i 1 to ${Math.Calc[${iter.Count[|]}+1]}
    /if (!${InvSlot[${iter.Arg[${i},|]}].Item.ID}) {
      /if (${${destVar}.Length}) {
        /varset ${destVar} ${${destVar}} ${iter.Arg[${i},|]}
      } else {
        /varset ${destVar} ${iter.Arg[${i},|]}
      }
    }
  /next i
  /if (!${InvSlot[offhand].Item.ID}) {
    /if (${InvSlot[mainhand].Item.ID} && ${InvSlot[mainhand].Item.Type.Find[2H]}) /return
    /if (${${destVar}.Length}) {
      /varset ${destVar} ${${destVar}} offhand
    } else {
      /varset ${destVar} offhand
    }
  }
/return

Sub ProcessShowArgs(EventLine, SlotName)
  /declare tempReply string local
  /declare i int local 0
  /declare currentArg string local
  /declare statSelect string local
  /call #argueString show| "${EventLine}"
  /for i 1 to ${Math.Calc[${Macro.Return.Count[ ]}+1]}
    /varset currentArg ${Macro.Return.Arg[${i}, ]}
    /varset statSelect
    /if (${Select[${currentArg},hp,mp,ac,agi,cha,dex,int,sta,str,wis]}) {
      /varset statSelect ${currentArg.Upper}
    } else /if (${Select[${currentArg},accuracy,attack,avoidance,haste,mana,shielding]}) {
      /varset statSelect ${currentArg.Left[1].Upper}${currentArg.Right[-1]}
    } else /if (${currentArg.Find[combat]}) {
      /varset statSelect CombatEffects
    }
    /if (${statSelect.Length}) {
      /varset tempReply ${tempReply} ${statSelect}: ${InvSlot[${SlotName}].Item.${statSelect}}
    }
  /next i
/return ${tempReply}

Sub ProcessItemTruth(SlotRef, TA)
  /if (${TA.Find[ or ]}) /varset TA ${TA.Replace[or,||]}
  /if (${TA.Find[ and ]}) /varset TA ${TA.Replace[and,&&]}
  /if (${TA.Find[hp]}) /varset TA ${TA.Replace[hp,${InvSlot[${SlotRef}].Item.HP}]}
  /if (${TA.Find[accuracy]}) /varset TA ${TA.Replace[accuracy,${InvSlot[${SlotRef}].Item.Accuracy}]}
  /if (${TA.Find[combateffects]}) /varset TA ${TA.Replace[combateffects,${InvSlot[${SlotRef}].Item.CombatEffects}]}
  /if (${TA.Find[combateff]}) /varset TA ${TA.Replace[combateff,${InvSlot[${SlotRef}].Item.CombatEffects}]}
  /if (${TA.Find[combat]}) /varset TA ${TA.Replace[combat,${InvSlot[${SlotRef}].Item.CombatEffects}]}
  /if (${TA.Find[mana]}) /varset TA ${TA.Replace[mana,${InvSlot[${SlotRef}].Item.Mana}]}
  /if (${TA.Find[atk]}) /varset TA ${TA.Replace[atk,${InvSlot[${SlotRef}].Item.Attack}]}
  /if (${TA.Find[attack]}) /varset TA ${TA.Replace[attack,${InvSlot[${SlotRef}].Item.Attack}]}
  /if (${TA.Find[avoidance]}) /varset TA ${TA.Replace[avoidance,${InvSlot[${SlotRef}].Item.Avoidance}]}
  /if (${TA.Find[haste]}) /varset TA ${TA.Replace[haste,${InvSlot[${SlotRef}].Item.Haste}]}
  /if (${TA.Find[shielding]}) /varset TA ${TA.Replace[shielding,${InvSlot[${SlotRef}].Item.Shielding}]}
  /if (${TA.Find[ac]}) /varset TA ${TA.Replace[ac,${InvSlot[${SlotRef}].Item.AC}]}
  /call #RequirePlugin MQ2Fusion
  /if (${Macro.Return}) {
    /return ${SafeCalc[${TA}].Int}
  } else {
    /return ${Math.Calc[${TA}].Int}
  }
/return
