ADC MIGRATION
-------------

This module is an extension of the Drupal Migrate module: https://www.drupal.org/project/migrate

We've used the framework provided in that contrib module for migrating content from the old ADC website: http://artesaniasdecolombia.com.co/PortalAC/General/template_index.jsf (which is based on Oracle JavaServer Faces) to the newer Drupal 7 website: http://34.225.9.157/ (soon to be launched)

There were lots of Content Types to be migrated, among them:

- News

- Polls

- Events

- Forums

- Glossary

For each one of the content types, we created an extended class of the base class Migration, for example, for the Events Content Type:

	class EventoNodeMigration extends Migration {

Each extended class contains the methods and properties for the migration process to work. Please take a look on the file adc_migration/migration/eventos.inc and see how the class definition was made.

The content was exported from the old Oracle database to CSV files, we created a set of menu callbacks and methods for reading these CSV files and creating the new nodes.

One example of the result of the migration process:

Old content: http://www.artesaniasdecolombia.com.co/PortalAC/Glosario/GlosarioUser.jsf

Migrated Content in Drupal website: http://34.225.9.157/glosario