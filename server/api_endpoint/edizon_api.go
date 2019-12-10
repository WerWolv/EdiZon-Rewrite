package main

import (
	"fmt"
	"os"
	"net/http"
	"encoding/json"
	"strings"
	"log"
	"io/ioutil"
)

var serverName, serverMotd string
var port string

const apiVersion string = "v1"

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

	http.HandleFunc("/",        base)
	http.HandleFunc("/version", version)

	http.HandleFunc("/" + apiVersion + "/get",           get)
	http.HandleFunc("/" + apiVersion + "/release",       release)
	http.HandleFunc("/" + apiVersion + "/notifications", notifications)


	http.ListenAndServe(":" + port, nil)
}

//SaveFile Struct containing save file information
type ReleaseData struct {
	Name          string	`json:"name"`
	Changelog     string    `json:"changelog"`
	Tag           string    `json:"tag"`
	Date          string	`json:"date"`
	DownloadCount uint32    `json:"download_count"`
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

	release := ReleaseData { Name: data["name"].(string), Changelog: strings.ReplaceAll(data["body"].(string), "\r\n", "\n"), Tag: data["tag_name"].(string), Date: data["published_at"].(string), DownloadCount: uint32(data["assets"].([]interface{})[0].(map[string]interface{})["download_count"].(float64)) }

	jsonResp, _ :=json.Marshal(release)

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