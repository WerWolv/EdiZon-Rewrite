import sys
import os
import json
import pprint

enUSLang = []
otherLang = []
newLang = []

def main():
    with open("../romfs/lang/en-US.json", "r") as enUSFile:
        enUSLang = json.load(enUSFile)
    with open("../romfs/lang/" + input("Enter a language to scan: ") + ".json", "r") as otherLangFile:
        otherLang = json.load(otherLangFile)
    
    newLang = enUSLang

    for otherStr in otherLang:
        newLang[otherStr] = otherLang[otherStr]

    for enStr in enUSLang:
        foundString = False
        for otherStr in otherLang:
            if enStr == otherStr:
                foundString = True
        
        if foundString == False:
            print("Missing Localization: " + enStr)
            newLang[enStr] = input("New value for " + enStr + ": ")

    with open("./newLang.json", "w") as newLangFile:
        json.dump(newLang, newLangFile, sort_keys = False, indent = 4)



if __name__ == "__main__":
    main()