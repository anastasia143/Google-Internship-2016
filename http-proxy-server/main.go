package main

import (
	"net/http"
	"net/http/httputil"
	"net/url"
	"time"
	"log"
)

func main() {
	var src, dst string

	dst = "127.0.0.1:8080"

	src = ":80"

	parsedUrl, err := url.Parse(dst)
	if err != nil {
		log.Fatal("Bad destination.")
	}

	// NewSingleHostReverseProxy returns a new ReverseProxy that routes URLs
	// to the scheme, host, and base path provided in target.
	reverseProxy := httputil.NewSingleHostReverseProxy(parsedUrl)
	
	server := &http.Server{
		Addr:           src,
		Handler:        reverseProxy,
		ReadTimeout:    10 * time.Second,
		WriteTimeout:   10 * time.Second,
		MaxHeaderBytes: 1 << 20,
	}
	log.Fatal(server.ListenAndServe())
}