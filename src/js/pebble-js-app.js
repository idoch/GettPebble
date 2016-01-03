Pebble.addEventListener('ready', function(e) {
    console.log('PebbleKit JS ready!');
  }
);


function getProfileInfo() {
  var result = {
    name: 'Ido Chetrit',
    points: 1090
  };
  
  var dictionary = {
    'KEY_PROFILE_INFO_NAME': result.name,
    'KEY_PROFILE_INFO_POINTS': result.points
  };

  // Send to Pebble
  Pebble.sendAppMessage(dictionary,
    function(e) {
      console.log('Profile info sent to Pebble successfully!');
    },
    function(e) {
      console.log('Error sending profile info to Pebble!');
    }
  );
}

Pebble.addEventListener('appmessage', function(e) {
    console.log('AppMessage received!');
    getProfileInfo();
  }
);