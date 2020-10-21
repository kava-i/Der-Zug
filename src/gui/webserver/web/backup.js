//Sends a request to create a backup to the server
function create_backup() {
  var path = window.location.pathname;
  var backup = path.substr(7);
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/create_backup");
  xhttp.send(backup);
  xhttp.onload = function(event){    
    var error_msg = document.getElementById("create_backup_error");
    if (xhttp.status != 200) {
      alert("Adding backup " + backup + " failed. Please try again later or " 
        + "contact an admin.");
    }
    else 
      alert("Successfully added backup: " + backup);
  }
}

//Opens a modal to show all existing backups
function restore_backup(backup) {

  var path = window.location.pathname;
  var request = new Object;
  request.world = path.substr(path.indexOf("_")+1);
  request.backup = backup;
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/restore_backup");
  xhttp.send(JSON.stringify(request));
  xhttp.onload = function(event){    
    var error_msg = document.getElementById("create_backup_error");
    if (xhttp.status != 200) {
      alert("Restoring backup " + backup + " failed. Please try again later or "
        + "contact an admin.");
    }
    else
      alert("Successfully restored backup: " + backup);
  }
}

//Delete Backup
function delete_backup(backup) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/delete_backup");
  xhttp.send(backup);
  xhttp.onload = function(event){    
    var error_msg = document.getElementById("create_backup_error");
    if (xhttp.status != 200) {
      alert("Deleting Backup " + backup + " failed. Please try again later or "
        + "contact an admin.");
    }
    else
      window.location = window.location;
  }
}
