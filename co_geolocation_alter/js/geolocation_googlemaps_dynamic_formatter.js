/**
 * @file
 * Javascript for Goole Map Dynamic API Formatter javascript code.
 *
 * Many thanks to Lukasz Klimek http://www.klimek.ws for the help
 */

(function($) {

    Drupal.geolocationGooglemaps = Drupal.geolocationGooglemaps || {};
    Drupal.geolocationGooglemaps.maps = Drupal.geolocationGooglemaps.maps || {};
    Drupal.geolocationGooglemaps.markers = Drupal.geolocationGooglemaps.markers || {};

    Drupal.behaviors.geolocationGooglemapsDynamicFormatter = {

        attach : function(context, settings) {
            var fields = settings.geolocationGooglemaps.formatters;
            // Work on each map
            $.each(fields, function(instance, data) {
                var instanceSettings = data.settings;
                $.each(data.deltas, function(d, delta) {

                    id = instance + "-" + d;

                    // Only make this once ;)
                    $(".geolocation-map").each(function(index){
                        if ($(this).attr('id') == ("geolocation-googlemaps-dynamic-" +  id) && !$(this).hasClass('geo-map-process')) {
                            var map_type;
                            var mapOptions;

                            var lat = delta.lat;
                            var lng = delta.lng;

                            if (navigator.geolocation) {
                                navigator.geolocation.getCurrentPosition(function(position) {
                                    if (typeof(Storage) !== "undefined") {
                                        localStorage.setItem("origen_lat", position.coords.latitude);
                                        localStorage.setItem("origen_lng", position.coords.longitude);
                                    } else {
                                        window.alert('No HTML5 support: ' + status);
                                    }
                                });
                            } else {
                                // Browser doesn't support Geolocation
                                window.alert('No geolocation support: ' + status);
                            }

                            if (typeof(Storage) !== "undefined") {
                                origen_lat = localStorage.getItem("origen_lat");
                                origen_lng = localStorage.getItem("origen_lng");
                            } else {
                                window.alert('No HTML5 support: ' + status);
                            }

                            var latLng = new google.maps.LatLng(lat, lng);
                            var origen = new google.maps.LatLng(origen_lat, origen_lng);
                            var infowindowDestino = new google.maps.InfoWindow;
                            var infowindowOrigen = new google.maps.InfoWindow;
                            var geocoderOrigen = new google.maps.Geocoder;

                            switch (instanceSettings.map_maptype) {
                                case 'satellite':
                                    map_type = google.maps.MapTypeId.SATELLITE;
                                    break;

                                case 'terrain':
                                    map_type = google.maps.MapTypeId.TERRAIN;
                                    break;

                                case 'hybrid':
                                    map_type = google.maps.MapTypeId.HYBRID;
                                    break;

                                default:
                                    map_type = google.maps.MapTypeId.ROADMAP;
                                    break;
                            }

                            mapOptions = {
                                zoom : parseInt(instanceSettings.map_zoomlevel),
                                center : latLng,
                                mapTypeId : map_type,
                                scrollwheel : instanceSettings.map_scrollwheel
                            };

                            // Create map
                            Drupal.geolocationGooglemaps.maps[id + '-' + index] = new google.maps.Map(this, mapOptions);

                            // Instantiate a directions service.
                            directionsService = new google.maps.DirectionsService,
                            directionsDisplay = new google.maps.DirectionsRenderer({
                                map: Drupal.geolocationGooglemaps.maps[id + '-' + index],
                            }),

                            markerA = new google.maps.Marker({
                                position: origen,
                                title: "",
                                label: "",
                                map: Drupal.geolocationGooglemaps.maps[id + '-' + index],
                            }),

                            markerB = new google.maps.Marker({
                                position: latLng,
                                title: "",
                                label: "",
                                map: Drupal.geolocationGooglemaps.maps[id + '-' + index],
                            });

                            geocoderOrigen.geocode({'location': origen}, function(results, status) {
                              if (status === 'OK') {
                                if (results[1]) {
                                  markerA = new google.maps.Marker({
                                    position: origen,
                                    title: "",
                                    label: "",
                                    map: Drupal.geolocationGooglemaps.maps[id + '-' + index],
                                  }),

                                  infowindowOrigen.setContent(results[1].formatted_address);
                                  infowindowOrigen.open(Drupal.geolocationGooglemaps.maps[id + '-' + index], markerA);
                                } else {
                                  window.alert('No results found');
                                }
                              } else {
                                window.alert('Geocoder failed due to: ' + status);
                              }
                            });

                            //labels
                            infowindowDestino.setContent(Drupal.settings.coGeolocationAlter.lugar);
                            infowindowDestino.open(Drupal.geolocationGooglemaps.maps[id + '-' + index], markerB);

                            // Create and place marker
                            Drupal.geolocationGooglemaps.markers[id + '-' + index] = new google.maps.Marker({
                                map : Drupal.geolocationGooglemaps.maps[id + '-' + index],
                                draggable : false,
                                icon : instanceSettings.marker_icon,
                                position : latLng
                            });

                            // get route from A to B
                            calculateAndDisplayRoute(directionsService, directionsDisplay, origen, latLng);
                            
                            $(this).addClass('geo-map-process');
                        };

                        function calculateAndDisplayRoute(directionsService, directionsDisplay, pointA, pointB) {
                            directionsService.route({
                                origin: pointA,
                                destination: pointB,
                                avoidTolls: true,
                                avoidHighways: false,
                                travelMode: google.maps.TravelMode.DRIVING
                            }, function (response, status) {
                                if (status == google.maps.DirectionsStatus.OK) {
                                    directionsDisplay.setDirections(response);
                                } else {
                                    window.alert('Directions request failed due to ' + status);
                                }
                            });
                        }
                    });

                    $('#boton-como-llegar').click(function(e) {
                        if (typeof(Storage) !== "undefined") {
                            origen_lat = localStorage.getItem("origen_lat");
                            origen_lng = localStorage.getItem("origen_lng");
                        } else {
                            window.alert('No HTML5 support: ' + status);
                        }

                        var destino_lat = delta.lat;
                        var destino_lng = delta.lng;

                        var url_google = "https://www.google.com/maps/dir/?api=1&origin=" + origen_lat + "," + origen_lng + "&destination=" + destino_lat + "," + destino_lng;
                        window.open(url_google, "_blank", "toolbar=yes,scrollbars=yes,resizable=yes,top=500,left=500,width=800,height=600");
                    });

                });
            });


        }
    };

}(jQuery));

