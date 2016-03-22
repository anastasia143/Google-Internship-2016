package main

import (
	"fmt"
	"net/http"
	"time"
	"log"
	"html/template"
)

// A single Broker is responsible for keeping a list of which clients (browsers)
// are currently attached and broadcasting events (messages) to those clients.
type Broker struct {

	// A map of clients, the keys of the map are the channels
	// over which we can push messages to attached clients.  (The values
	// are just booleans and are meaningless.)
	clients map[chan string]bool

	// Channel into which new clients can be pushed
	newClients chan chan string

	// Channel into which disconnected clients should be pushed
	defunctClients chan chan string

	// Channel into which messages are pushed to be broadcast out
	// to attached clients.
	messages chan string
}

// Handler for the main page, which we wire up to the
// route at "/" below in `main`.
func MainPageHandler(writer http.ResponseWriter, reader *http.Request) {

	// Golang's ServeMux matches only the prefix of the request URL.
	// Here we insist the path is just "/".
	if reader.URL.Path != "/" {
		writer.WriteHeader(http.StatusNotFound)
		return
	}

	// Read in the template with SSE JavaScript code.
	temp, err := template.ParseFiles("index.html")
	if err != nil {
		log.Fatal("Template parsing error.")
	}

	// Render the template.
	temp.Execute(writer, "Name")

	// Done.
	log.Println("Finished HTTP request at ", reader.URL.Path)
}

// This Broker method starts a new goroutine.  It handles
// the addition & removal of clients, as well as the broadcasting
// of messages out to clients that are currently attached.
func (broker *Broker) Start() {

	// Start a goroutine
	go func() {
		// Loop endlessly
		for {
			// Block until we receive from one of the
			// three following channels.
			select {

			case newClient := <-broker.newClients:

			// There is a new client attached and we
			// want to start sending them messages.
				broker.clients[newClient] = true
				log.Println("Added new client")

			case defunctClient := <-broker.defunctClients:

			// A client has dettached and we want to
			// stop sending them messages.
				delete(broker.clients, defunctClient)
				close(defunctClient)
				log.Println("Removed client")

			case msg := <-broker.messages:

			// There is a new message to send.  For each
			// attached client, push the new message
			// into the client's message channel.
				for client, _ := range broker.clients {
					client <- msg
				}
				log.Printf("Broadcast message to %d clients", len(broker.clients))
			}
		}
	}()
}

// This Broker method handles and HTTP request at the "/events/" URL.
func (b *Broker) ServeHTTP(writer http.ResponseWriter, reader *http.Request) {

	// Make sure that the writer supports flushing.

	flusher, ok := writer.(http.Flusher)
	if !ok {
		http.Error(writer, "Streaming unsupported.", http.StatusInternalServerError)
		return
	}

	// Create a new channel, over which the broker can
	// send this client messages.
	messageChan := make(chan string)

	// Add this client to the map of those that should
	// receive updates
	b.newClients <- messageChan

	// Listen to the closing of the http connection via the CloseNotifier
	notify := writer.(http.CloseNotifier).CloseNotify()
	go func() {
		<-notify
		// Remove this client from the map of attached clients
		// when `EventHandler` exits.
		b.defunctClients <- messageChan
		log.Println("HTTP connection just closed.")
	}()

	// Set the headers related to event streaming.
	writer.Header().Set("Content-Type", "text/event-stream")
	writer.Header().Set("Cache-Control", "no-cache")
	writer.Header().Set("Connection", "keep-alive")

	// Don't close the connection, instead loop 10 times, sending messages and flushing the response each time
	// there is a new message to send along.
	//
	// NOTE: we could loop endlessly; however, then you  could not easily detect clients
	// that dettach and the server would continue to send them messages long after
	// they're gone due to the "keep-alive" header.  One of the nifty aspects of SSE is that clients
	// automatically reconnect when they lose their connection.
	// (A better way to do this is to use the CloseNotifier)

	for {
		// Read from our messageChan.
		msg, open := <-messageChan

		if !open {
			// If our messageChan was closed, this means that the client has disconnected.
			break
		}

		// Write to the ResponseWriter
		fmt.Fprintf(writer, "data: Message: %s\n\n", msg)

		// Flush the response.  This is only possible if
		// the response supports streaming.
		flusher.Flush()
	}

	// Done.
	log.Println("Finished HTTP request at ", reader.URL.Path)
}

func main() {
	// Make a new Broker instance
	broker := &Broker{
		make(map[chan string]bool),
		make(chan (chan string)),
		make(chan (chan string)),
		make(chan string),
	}

	// Start processing events
	broker.Start()

	// Make broker the HTTP handler for "/events/".
	// We can do this because broker has a ServeHTTP method.
	// That method is called in a separate goroutine for each request to "/events/".
	http.Handle("/events/", broker)

	// Generate a constant stream of events that get pushed into the Broker's
	// messages channel and are then broadcast out to any clients that are attached.
	go func() {
		for i := 0; ; i++ {

			// Create a little message to send to clients,
			// including the current time.
			broker.messages <- fmt.Sprintf("%d - the time is %v", i, time.Now())

			// Print a nice log message and sleep for 5s.
			log.Printf("Sent message %d ", i)
			time.Sleep(5 * 1e9)

		}
	}()

	// When we get a request at "/", call `MainPageHandler` in a new goroutine.
	http.Handle("/", http.HandlerFunc(MainPageHandler))

	// Start the server and listen forever on port 80.
	http.ListenAndServe(":80", nil)
}