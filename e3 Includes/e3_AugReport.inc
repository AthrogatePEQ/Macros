#Event AugReport "[MQ2] AugReport#2#"
Sub Event_AugReport(line, chatSender, args)
  /call #verifyEvent "${chatSender}" "${line}"
  /if (!${Macro.Return}) /return

  /echo Gathering Augments Report

  /declare i int local
  /declare j int local
  /declare AugReturn string local
  /declare AugCount int local 0

  /for i 0 to 20
    /call GetAugDetails "InvSlot[${i}].Item" "${InvSlot[${i}].Name}"
    /declare AugReturn${i} string local ${Macro.Return}
    /varcalc AugCount ${AugCount}+${AugReturn${i}.Count[|]}
  /next i

  /declare AR:AugArray[${AugCount}] string outer
  /declare AugIter int local 1
  /for i 0 to 20
    /if (${AugReturn${i}.Count[|]}) {
      /for j 1 to ${AugReturn${i}.Count[|]}
        /varset AR:AugArray[${AugIter}] ${AugReturn${i}.Arg[${j},|]}
        /varcalc AugIter ${AugIter}+1
      /next j
    }
  /next i

  /call #QuickSort AR:AugArray 1 ${AugCount} .Arg[2,:]
  /call #QuickSort AR:AugArray 1 ${AugCount} .Arg[1,:]

  /declare AugData string local
  /varcalc i ${AugCount}
  :NextAug
  /varset AugData ${AR:AugArray[${i}]}
  /varset AugData ${AugData.Right[-${AugData.Find[:]}]}
  /varset AugData ${AugData.Right[-${AugData.Find[:]}]}
  /echo ${AugData}
  /varcalc i ${i} - 1
  /if (${i} > 0) /goto :NextAug

  /deletevar AR:AugArray
/return

Sub GetAugDetails(ItemRef, LocRef)
  /declare DisplayAugType bool local true
  /declare DisplayEffects bool local false
  /declare i int local
  /declare j int local
  /declare ret string local
  /declare hp_mana string local
  /declare RelevantStats string local AC~AC|Avoidance~Avoid|Shielding~Shielding|Accuracy~Acc|CombatEffects~Combat|Attack~Atk|Haste~Haste|
  /declare RelevantAugTypes string local |7|8|
  /declare StatCount int local ${RelevantStats.Count[|]}
  /declare StatName string local
  /declare StatAlias string local
  /declare AugRef string local
  /declare AugType int local


  /if (${${ItemRef}.Augs}) {
    /for i 1 to ${${ItemRef}.Augs}
      /varset AugRef ${ItemRef}.Item[${i}]
      /varcalc AugType ${${ItemRef}.AugSlot${i}}
      /if (${RelevantAugTypes.Find[|${AugType}|]}) {
        /if (${Bool[${${AugRef}}]}) {
            /varset hp_mana ${${AugRef}.HP}:${${AugRef}.Mana}
            |/varset ret ${ret}${hp_mana}:${PadR[${LocRef}:,12, ]}
            |/varset ret ${ret} H: ${PadL[${${AugRef}.HP},3, ]}
            |/varset ret ${ret} M: ${PadL[${${AugRef}.Mana},3, ]}
            /varset ret ${ret}${hp_mana}:${LocRef}:
            /varset ret ${ret} H: ${${AugRef}.HP}
            /varset ret ${ret} M: ${${AugRef}.Mana}

            /if (${DisplayAugType}) /varset ret ${ret} Type: ${AugType}
            /for j 1 to ${StatCount}
              /varset StatName ${RelevantStats.Arg[${j},|].Arg[1,~]}
              /varset StatAlias ${RelevantStats.Arg[${j},|].Arg[2,~]}
              /if (${${AugRef}.${StatName}} != 0) {
                /varset ret ${ret} ${StatAlias}: ${${AugRef}.${StatName}}
              }
            /next j
            |/if (${DisplayEffects}) {
            |  /if (${${AugRef}.EffectCount}) {
            |    /varset ret ${ret} Effects:
            |    /for j 1 to ${${AugRef}.EffectCount}
            |      /varset ret ${ret} [${${AugRef}.Effects[${j}].Name}]
            |    /next j
            |  }
            |}
            /varset ret ${ret}|
        } else {
          |/varset ret ${ret}0:0:${PadR[${LocRef}:,12, ]} ${PadR[EMPTY,13, ]} Type: ${AugType}|
          /varset ret ${ret}0:0:${LocRef} EMPTY Type: ${AugType}|
        }
      }
    /next i
  }
/return ${ret}

Sub AugReport_Background_Events
  /doevents AugReport
/return

Sub AugReport_Aliases
  /alias /augreport /echo augreport
/return

Sub AugReport_Setup
/return

Sub AugReport_CharacterSettings
/return

sub AugReport_MacroSettings
/return