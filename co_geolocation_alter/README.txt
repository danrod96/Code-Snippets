CO GEOLOCATION
--------------

The purpose of this module is to include custom changes for the geolocation module: https://www.drupal.org/project/geolocation

Here's a description of the functionality included in the geolocation module:

Geolocation Module provides a field type to store geographical locations as pairs of latitude and longitude (lan,lng). The Drupal 8 version also provides a views proximity search plugin. Geolocation Field can be used with all fieldable entities like nodes, users, comments, taxonomy terms, etc.

We found out that the js included for displaying the map coords in Google Maps was somewhat limited to what we needed on a project (http://www.comfandi.com.co), so what I did is create a new module for overriding that JS file (using a call to co_geolocation_alter_js_alter) and include my own JS file.

There are some content types on that website (events, stores) which have the geolocation field (storing the lat and lng information), and then a custom tpl will paint the Google Map based on the information from the geolocation field.

My extra JS draws a route from the device's current position (using HTML5 geolocation) to the final destination indicated on the lat and lng coordinates and displays some useful information.

There is also a button next to the map (I used the field--field-geolocation.tpl.php for placing that button), if you click on that button, it will take you to the google map window with detailed instructions for getting there (by plane, train, road, etc).

You can see that functionality in action here (test site only):

https://34.193.52.202/portal_comfandi/persona/cali/eventos/el-valle-suena-en-vivo-puerto-candelaria-palmira

https://34.193.52.202/portal_comfandi/persona/cali/home/supermercado-y-drogueria-comfandi-express-canasgordas

don't forget to allow chrome to access your location.

