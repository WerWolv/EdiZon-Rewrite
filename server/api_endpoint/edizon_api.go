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
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"strings"
)

var serverName, serverMotd string
var port string

const apiVersion string = "v4"

func main() {
	var args []string = os.Args[1:]

	if len(args) > 2 {
		fmt.Println("Usage: ./edizon_api [<port>]")
		return
	}

	if len(args) == 2 {
		port = args[0]
	} else {
		port = "1337"
	}

	http.HandleFunc("/", base)
	http.HandleFunc("/version", version)

	http.HandleFunc("/"+apiVersion+"/get", get)
	http.HandleFunc("/"+apiVersion+"/release", release)
	http.HandleFunc("/"+apiVersion+"/notifications", notifications)
	http.HandleFunc("/"+apiVersion+"/official_providers", officialProviders)

	http.ListenAndServe(":"+port, nil)
}

//ReleaseData Struct containing reslease information
type ReleaseData struct {
	Name          string `json:"name"`
	Changelog     string `json:"changelog"`
	Tag           string `json:"tag"`
	Date          string `json:"date"`
	DownloadCount uint32 `json:"download_count"`
}

func base(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte("EdiZon API endpoint"))
}

func version(w http.ResponseWriter, r *http.Request) {
	w.Header().Add("Content Type", "application/json")
	w.Write([]byte("{ \"version\" : \"" + apiVersion + "\" }"))
}

func get(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte("EdiZon API endpoint"))
}

func release(w http.ResponseWriter, r *http.Request) {
	resp, err := http.Get("https://api.github.com/repos/WerWolv/EdiZon/releases/latest")

	if err != nil {
		w.Header().Add("Content Type", "application/json")
		w.Write([]byte("{ \"error\" : \"Request failed\" }"))
		return
	}

	defer resp.Body.Close()
	receivedData, err := ioutil.ReadAll(resp.Body)

	var data map[string]interface{}
	if err := json.Unmarshal(receivedData, &data); err != nil {
		w.Header().Add("Content Type", "application/json")
		w.Write([]byte("{ \"error\" : \"Request failed\" }"))
		log.Fatal(err)
		return
	}

	release := ReleaseData{Name: data["name"].(string), Changelog: strings.ReplaceAll(data["body"].(string), "\r\n", "\n"), Tag: data["tag_name"].(string), Date: data["published_at"].(string), DownloadCount: uint32(data["assets"].([]interface{})[0].(map[string]interface{})["download_count"].(float64))}

	jsonResp, _ := json.Marshal(release)

	w.Write(jsonResp)
}

func notifications(w http.ResponseWriter, r *http.Request) {
	w.Header().Add("Content Type", "application/json")

	data, err := ioutil.ReadFile("./data/notifications.json")

	if err != nil {
		w.Write([]byte("{ \"error\" : \"Request failed\" }"))
		return
	}

	w.Write(data)
}

func officialProviders(w http.ResponseWriter, r *http.Request) {
	w.Header().Add("Content Type", "application/json")

	data, err := ioutil.ReadFile("./data/official_providers.json")

	if err != nil {
		w.Write([]byte("{ \"error\" : \"Request failed\" }"))
		return
	}

	w.Write(data)
}
