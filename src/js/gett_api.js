Pebble.addEventListener('ready', function(e) {
    console.log('PebbleKit JS ready!');
  }
);

function getProfileInfo () {
  var result = {
    name: 'Ido Chetrit',
    points: 1090
  };
  
  var dictionary = {
    'KEY_PROFILE_INFO': result
  };

  // Send to Pebble
  Pebble.sendAppMessage(dictionary,
    function(e) {
      console.log('Weather info sent to Pebble successfully!');
    },
    function(e) {
      console.log('Error sending weather info to Pebble!');
    }
  );
}

Pebble.addEventListener('appmessage', function(e) {
    console.log('AppMessage received!');
    getProfileInfo();
  }
);