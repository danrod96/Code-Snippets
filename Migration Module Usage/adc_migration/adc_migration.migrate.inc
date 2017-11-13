<?php

/**
 * @file
 * Because the name of this file is the module name plus '.migrate.inc', when
 * hook_migrate_api is invoked by the Migrate module this file is automatically
 * loaded - thus, you don't need to implement your hook in the .module file.
 */

/**
 * To define a migration process from a set of source data to a particular
 * kind of Drupal object (for example, a specific node type), you define
 * a class derived from Migration. You must define a constructor to initialize
 * your migration object.
 *
 * For your classes to be instantiated so they can be used to import content,
 * you must register them - look at migrate_example.migrate.inc to see how
 * registration works. Rmorelos
 * ight now, it's important to understand that each
 * migration will have a unique "machine name", which is displayed in the UI
 * and is used to reference the migration in drush commands.
 *
 * In any serious migration project, you will find there are some options
 * which are common to the individual migrations you're implementing. You can
 * define an abstract intermediate class derived from Migration, then derive your
 * individual migrations from that, to share settings, utility functions, etc.
 */


/*
 * You must implement hook_migrate_api(), setting the API level to 2, if you are
 * implementing any migration classes. If your migration application is static -
 * that is, you know at implementation time exactly what migrations must be
 * instantiated - then you should register your migrations here. If your
 * application is more dynamic (for example, if selections in the UI determine
 * exactly what migrations need to be instantiated), then you would register
 * your migrations using registerMigration() - see migrate_example_baseball for
 * more information.
 */

function adc_migration_migrate_api() {
  // Usually field mappings are established by code in the migration constructor -
  // a call to addFieldMapping(). They may also be passed as arguments when
  // registering a migration - in this case, they are stored in the database
  // and override any mappings for the same field in the code. To do this,
  // construct the field mapping object and configure it similarly to when
  // you call addFieldMapping, and pass your mappings as an array below.
  $translate_mapping = new MigrateFieldMapping('translate', NULL);
  $translate_mapping->defaultValue(0);

  $api = array(
    // Required - tells the Migrate module that you are implementing version 2
    // of the Migrate API.
    'api' => 2,
    // machine name of the group, which can be used in Drush:
    //  drush migrate-import --group=wine
    // The title is a required argument which is displayed for the group in the
    // UI. You may also have additional arguments for any other data which is
    // common to all migrations in the group.
    'groups' => array(
      'article' => array(
         'title' => t('Importar Contenido Tipo Noticia '),
      ),
      'page' => array(
        'title' => t('Importar Contenido Tipo Publicación '),
      ),
      // 'body' => array(
      //   'title' => t('Importar Cuerpo de Contenido Tipo Publicación y Noticia '),
      // ),
      'artesanos' => array(
        'title' => t('Importar Usuarios Artesanos '),
      ),
      // 'internos' => array(
      //   'title' => t('Importar Usuarios Internos '),
      // ),
      // 'geografiacodigopais' => array(
      //   'title' => t('Importar Geografia <Código de pais> '),
      // ),
      // 'geografiacodigodepartamento' => array(
      //   'title' => t('Importar Geografia <Código de Departamento> '),
      // ),
      // 'geografiacodigociudad' => array(
      //   'title' => t('Importar Geografia <Código de Ciudad> '),
      // ),
      'directorios' => array(
         'title' => t('Importar Contenido Tipo Directorios '),
      ),
      // 'clasificados' => array(
      //   'title' => t('Importar Contenido Tipo Clasificados '),
      // ),
      'hv' => array(
         'title' => t('Importar Contenido Tipo HV '),
      ),
      'contratos' => array(
         'title' => t('Importar Contenido Tipo Contratos '),
      ),
      // 'foros' => array(
      //   'title' => t('Importar Contenido Tipo foros '),
      // ),
      // 'faqs' => array(
      //   'title' => t('Importar Contenido Tipo preguntas frecuentes '),
      // ),
      // 'plandecompras' => array(
      //   'title' => t('Importar Contenido Plan de Compras '),
      // ),
      'taller' => array(
        'title' => t('Importar Contenido Taller'),
      ),
      'productostaller' => array(
        'title' => t('Importar Contenido Productos Taller'),
      ),
      // 'encuestas' => array(
      //   'title' => t('Importar Contenido Encuestas'),
      // ),
      'eventos' => array(
        'title' => t('Importar Contenido Eventos'),
      ),
      // 'glosario' => array(
      //   'title' => t('Importar Taxonomia glosario'),
      // ),
      'estado' => array(
        'title' => t('Importar estado de Contenido Tipo Publicación y Noticia '),
       ),
      'vinculos' => array(
        'title' => t('Importar vinculos realcionados de Publicación y Noticia '),
       ),

    ),
    // Here we register the individual migrations. The keys (BeerTerm, BeerUser,
    // etc.) are the machine names of the migrations, and the class_name
    // argument is required. The group_name is optional (defaulting to 'default')
    // but specifying it is a best practice.
    'migrations' => array(
      // 'Clasificados' => array(
      //   'class_name' => 'ClasificadosNodeMigration',
      //   'group_name' => 'clasificados'
      // ),
      'Noticias' => array(
         'class_name' => 'NoticiaNodeMigration',
         'group_name' => 'article'
      ),
      'Publicaciones' => array(
         'class_name' => 'PublicacionNodeMigration',
         'group_name' => 'page'
      ),
      // 'Body' => array(
      //   'class_name' => 'BodyNodeMigration',
      //   'group_name' => 'body'
      // ),
      'Directorios' => array(
         'class_name' => 'DirectoriosNodeMigration',
         'group_name' => 'directorios'
      ),
      'Artesanos' => array(
        'class_name' => 'ArtesanosNodeMigration',
         'group_name' => 'artesanos'
      ),
      // 'Internos' => array(
      //   'class_name' => 'InternosNodeMigration',
      //   'group_name' => 'internos'
      // ),
      // 'GeografiaCodigoPais' => array(
      //   'class_name' => 'GeografiaCodigoPaisMigration',
      //   'group_name' => 'geografiacodigopais'
      // ),
      // 'GeografiaCodigoDepartamento' => array(
      //   'class_name' => 'GeografiaCodigoDepartamentoMigration',
      //   'group_name' => 'geografiacodigodepartamento'
      // ),
      // 'GeografiaCodigoCiudad' => array(
      //   'class_name' => 'GeografiaCodigoCiudadMigration',
      //   'group_name' => 'geografiacodigociudad'
      // ),
      'HvMigration' => array(
        'class_name' => 'HvNodeMigration',
        'group_name' => 'hv'
      ),
      'ContratosMigration' => array(
         'class_name' => 'ContratosNodeMigration',
         'group_name' => 'contratos'
      ),
      // 'Foros' => array(
      //   'class_name' => 'ForosNodeMigration',
      //   'group_name' => 'foros'
      // ),
      // 'Faqs' => array(
      //   'class_name' => 'FaqNodeMigration',
      //   'group_name' => 'faqs'
      // ),
      // 'PlanDeComprasMigration' => array(
      //   'class_name' => 'PlanDeComprasNodeMigration',
      //   'group_name' => 'plandecompras'
      // ),
      'TallerMigration' => array(
        'class_name' => 'TallerNodeMigration',
        'group_name' => 'taller'
      ),
      
      'ProductosTallerMigration' => array(
        'class_name' => 'ProductosTallerNodeMigration',
        'group_name' => 'productostaller'
      ),
      // 'Encuestas' => array(
      //   'class_name' => 'EncuestaNodeMigration',
      //   'group_name' => 'encuestas'
      // ),
      'Eventos' => array(
        'class_name' => 'EventoNodeMigration',
        'group_name' => 'eventos'
      ),
      'Estado' => array(
         'class_name' => 'EstadoNodeMigration',
         'group_name' => 'estado'
      ),
      'Vinculos' => array(
         'class_name' => 'VinculoRelacionadoNodeMigration',
         'group_name' => 'vinculos'
      ),
      
      // 'Glosario' => array(
      //   'class_name' => 'GlosarioNodeMigration',
      //   'group_name' => 'glosario'
      // ),
    ),
  );
  return $api;
}
