//Function to send request, to check whether game is still running
const TEXT_ADVENTURE_PORT = 4489;

function check_running() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/check_running");
  xhttp.send(window.location.pathname);
  
  xhttp.onload = function(event){
    document.getElementById("modal_log").style.display = "block";
    if (xhttp.status == 200)
      document.getElementById("check_msg").innerHTML = "Game is running as accpected!";
    else
      document.getElementById("check_msg").innerHTML = "Game is not running!";
  }
}

function get_log(x) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/get_"+x+"_log");
  console.log("sending request");
  xhttp.send(window.location.pathname);

  //Reset fields
  document.getElementById("display_log").innerHTML = "";
  document.getElementById("get_log").style.display = "block";

  xhttp.onload = function(event){
    console.log("Got response: ", this.responseText);
    if (xhttp.status == 200) {
      // Make sure that log-modal is visible
      document.getElementById("modal_log").style.display = "block";
      document.getElementById("log_div").style.display="block";
      document.getElementById("display_log").innerHTML = this.responseText;
      document.getElementById("get_log").style.display = "none";
    }
    else
      document.getElementById("check_msg").innerHTML = "failed getting log.";
  }
}

async function try_game(creator, world_name) {
  notify("Closing running game...");
  end(creator, world_name, true);
  await new Promise(r => setTimeout(r, 1800));
  notify("Starting game...");
  run(creator, world_name);
}

async function run(creator, world_name) {
  // Check if game is already running.
  const data = new Object({"creator": creator, "world_name": world_name});
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/running");
  xhttp.send(JSON.stringify(data));
  xhttp.onload = function(event){
    // Game already running 
    if (this.status == 200) {
      notify("Game already running.");
      return;
    }
  }

  // If not already running, send request to start game.
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/start");
  xhttp.send(JSON.stringify(data));

  //Redirect to game
  xhttp.onload = function(event){
    if (this.status != 200)
      notify("Unkown problem starting game: " + this.status);
    // If Successfully started, redirect to game-page.
    else {
      var hostname = window.location.hostname;
      window.open("http://" + hostname+ ":" + TEXT_ADVENTURE_PORT + "/" + creator + "/" + world_name);
    }
  }
}

async function end(creator, world_name, silent=false) {
  // Check if game is already running.
  const data = new Object({"creator": creator, "world_name": world_name});
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/close");
  xhttp.send(JSON.stringify(data));
  xhttp.onload = function(event){
    // Game already running 
    if (this.status == 200) {
      notify("Game closed.");
      return;
    }
    else if (this.status == 204) {
      notify("Game already closed.");
      return;
    }
    else {
      notify("Something went wring: status-code=" + this.status);
    }
  }
  if (!silent)
    document.getElementById("modal_log").style.display = "block";
}
