/**
 * Copyright (C) 2019 - 2020 WerWolv
 *
 * This file is part of EdiZon.
 *
 * EdiZon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * EdiZon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EdiZon.  If not, see <http://www.gnu.org/licenses/>.
 */

package main

import (
	"encoding/binary"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"strings"

	"github.com/go-restruct/restruct"
)

var serverName, serverMotd string
var port string

const apiVersion string = "v1"

func main() {
	var args []string = os.Args[1:]

	if len(args) < 2 || len(args) > 3 {
		fmt.Println("Usage: ./save_provider <name> <motd> [<port>]")
		return
	}

	serverName = args[0]
	serverMotd = args[1]

	if len(args) == 3 {
		port = args[2]
	} else {
		port = "1234"
	}

	http.HandleFunc("/", base)
	http.HandleFunc("/name", name)
	http.HandleFunc("/motd", motd)
	http.HandleFunc("/version", version)

	http.HandleFunc("/"+apiVersion+"/list", list)
	http.HandleFunc("/"+apiVersion+"/get", get)
	http.HandleFunc("/"+apiVersion+"/icon", icon)

	http.ListenAndServe(":"+port, nil)
}

//SaveFile Struct containing save file information
type SaveFile struct {
	FileName string `json:"fileName"`
	Date     string `json:"date"`
	TitleID  string `json:"titleID"`
}

//SaveFileList Struct containing a list of SaveFiles
type SaveFileList struct {
	SaveFiles []SaveFile `json:"saveFiles"`
}

// SaveFileHeader Struct containing the header data of a save file
type SaveFileHeader struct {
	Magic   uint32 `struct:"uint32"`
	TitleID uint64 `struct:"uint64"`
}

func base(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte("EdiZon save file provider server"))
}

func name(w http.ResponseWriter, r *http.Request) {
	w.Header().Add("Content Type", "application/json")
	w.Write([]byte("{ \"name\" : \"" + serverName + "\" }"))
}

func motd(w http.ResponseWriter, r *http.Request) {
	w.Header().Add("Content Type", "application/json")
	w.Write([]byte("{ \"motd\" : \"" + serverMotd + "\" }"))
}

func icon(w http.ResponseWriter, r *http.Request) {
	data, err := ioutil.ReadFile(string("./icon.jpg"))

	if err == nil {
		w.Header().Add("Content Type", "image/jpg")
		w.Write(data)
	} else {
		w.Header().Add("Content Type", "application/json")
		w.Write([]byte("{ \"error\" : \"File not found\"}"))
	}
}

func list(w http.ResponseWriter, r *http.Request) {
	files, err := ioutil.ReadDir("./saves")

	if err != nil {
		return
	}

	if len(files) != 0 {
		var vec SaveFileList
		i := 0
		for _, file := range files {
			var header SaveFileHeader
			f, err := os.Open("./saves/" + file.Name())

			if err != nil {
				continue
			}

			var data = make([]byte, 12)
			_, err = f.Read(data)

			if err != nil {
				continue
			}

			restruct.Unpack(data, binary.LittleEndian, &header)

			if header.Magic != 0x4E5A4445 {
				continue
			}

			vec.SaveFiles = append(vec.SaveFiles, SaveFile{FileName: file.Name(), Date: file.ModTime().Format("15:04 - 2. January 2006"), TitleID: fmt.Sprintf("%016x", header.TitleID)})
			i++
		}

		jsonStr, _ := json.Marshal(vec)
		w.Write(jsonStr)
	} else {
		w.Write([]byte("{ }"))
	}

}

func get(w http.ResponseWriter, r *http.Request) {
	keys, ok := r.URL.Query()["fileName"]

	if !ok || len(keys) < 1 || strings.Contains(keys[0], "/") {
		w.Header().Add("Content Type", "application/json")
		w.Write([]byte("{ \"error\" : \"Invalid 'fileName' parameter\"}"))
		return
	}

	data, err := ioutil.ReadFile("./saves/" + keys[0])

	if err == nil {
		w.Header().Set("Content-Disposition", "attachment; filename="+keys[0])
		w.Write(data)
	} else {
		w.Header().Add("Content Type", "application/json")
		w.Write([]byte("{ \"error\" : \"File not found\"}"))
	}
}

func version(w http.ResponseWriter, r *http.Request) {
	w.Header().Add("Content Type", "application/json")
	w.Write([]byte("{ \"version\" : \"" + apiVersion + "\" }"))
}
