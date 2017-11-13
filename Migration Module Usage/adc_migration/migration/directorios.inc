<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);

/*
 * @file
 * The DirectoriosNodeMigration uses the  directorios.csv files as source
 * and creates Drupal nodes of type 'directorio' as destination.
 */

require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class DirectoriosNodeMigration extends Migration {
	
  public $debug = false;
  public $ignore_file = false;
  public $conteo_nocorreos = 0;
  public $conteo_nodosexistentes = 0;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('CSV feed (pull) of Nodes tipo directorio');

    $fields = array(
	    0 => array('id_de_directorio', 'id_directorio'),
	    1 => array('login_de_usuario', 'id_usuario_artesano'),
      2 => array('correo_usuario_artesano', 'correo_usuario_artesano'),
      3 => array('categoria', 'categoria_directorio'),
      4 => array('nombre_categoria_directorio', 'nombre_categoria_directorio'),
      5 => array('perfil_directorio', 'perfil_directorio'),
      6 => array('nombre_perfil_directorio', 'nombre_perfil_directorio'),
      7 => array('estado_workflow', 'estado_workflow'),
      8 => array('nombre_estado_workflow', 'nombre_estado_workflow'),
      9 => array('estado', 'estado'),
     10 => array('usuario_creacion', 'usuario_creacion'),
     11 => array('fecha_crea', 'fecha_creacion'),
     12 => array('usuario_modificacion', 'usuario_modificacion'),
     13 => array('fecha_modifica', 'fecha_modificacion'),
     14 => array('descripcion_del_directorio', 'descripcion'),
    );

    //The Source of the import
    //$path = variable_get('directorio_csv_path', 'public://migration/directorio.csv');
    $path = DRUPAL_ROOT . '/sites/default/files/migration/directorios.csv';
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type directorio
    $this->destination = new MigrateDestinationNode('directorio');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_de_directorio' => array('type' => 'varchar', 'length' => 255, 'description' => 'ID DEL DIRECTORIO.', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());

    
    // Field id de directorio <Texto>;
    $this->addFieldMapping('field_id_de_directorio', 'id_de_directorio');

    // Make the mappings
    // Field Title <Texto>;
    $this->addFieldMapping('title', 'title'); // Field Title 

    // Descripción 
    $this->addFieldMapping('body', 'descripcion_del_directorio');

    // Categoria de Directorio
    $this->addFieldMapping('field_categoria_directorio', 'categoria'); // Field Categoria de Directorio
    $this->addFieldMapping('field_categoria_directorio:source_type')->defaultValue('tid');

    // Palabras Claves
    $this->addFieldMapping('field_etiquetas_directorio', 'etiquetas_directorio');
    $this->addFieldMapping('field_etiquetas_directorio:create_term')
         ->defaultValue(TRUE);
    $this->addFieldMapping('field_etiquetas_directorio:ignore_case')
         ->defaultValue(TRUE);

    // Estado del directorio 
    $this->addFieldMapping('field_estado_directorio', 'estado_equivalencia');

    // Field Created Date
    $this->addFieldMapping('created', 'fecha_crea');

    // Field Changed Date
    $this->addFieldMapping('changed', 'fecha_modifica');

    // Field Status
    $this->addFieldMapping('status', 'estado');
    
    // Field UID
    $this->addFieldMapping('uid', 'uid');

    // Unmapped destination fields
    // Some conventions we use here: with a long list of fields to ignore, we
    // arrange them alphabetically, one distinct field per line (although
    // subfields of the same field may be grouped on the same line), and indent
    // subfields to distinguish them from top-level fields.
    $this->addUnmigratedDestinations(array(
      'is_new',
      'log',
    ));

  }

  // The prepareRow() method is called by the source class next() method, after loading the data row. The argument $row
  // is a stdClass object containing the raw data as provided by the source. There are two primary reasons to implement
  // prepareRow():
  // 1) To modify the data row before it passes through any further methods and handlers: for example, fetching related
  // data, splitting out source fields, combining or creating new source fields based on some logic.
  // 2) To conditionally skip a row (by returning FALSE).
  public function prepareRow($current_row) {
    // Always include this fragment at the beginning of every prepareRow()
    // implementation, so parent classes can ignore rows.
    if (parent::prepareRow($current_row) === FALSE) {
      return FALSE;
    }

    // Source ID
    $current_row->id_de_directorio = str_replace(".", "", $current_row->id_de_directorio);
    $current_row->id_de_directorio = str_replace(",", "", $current_row->id_de_directorio);

    //checkear si el ID de directorio ya esta
    $result = db_query("SELECT entity_id FROM field_data_field_id_de_directorio WHERE field_id_de_directorio_value = :id_directorio LIMIT 1", array(":id_directorio" => $current_row->id_de_directorio))->fetchAssoc();

    if (!empty($result['entity_id'])){
      drupal_set_message(t("El directorio @dir ya esta registrado", array("@dir" => $current_row->id_de_directorio)), "status");
      return FALSE;
    } else {
      drupal_set_message(t("Registrando el siguiente nuevo directorio: @dir", array("@dir" => $current_row->id_de_directorio)), "status");
    }

    if(!valid_email_address($current_row->correo_usuario_artesano)){
      drupal_set_message(t("El siguiente correo NO ES VALIDO: @correo", array("@correo" => $current_row->correo_usuario_artesano)), "status");
      return FALSE;
    }else{
      $user = user_load_by_mail($current_row->correo_usuario_artesano);
      if($user == FALSE){
        drupal_set_message(t("El siguiente correo no esta registrado como artesano: @correo", array("@correo" => $current_row->correo_usuario_artesano)), "status");
        return FALSE;
      }else{
        $current_row->uid = $user->uid; // User ID
        $title = array();
        if(isset($user->field_primer_nombre['und'][0]['value'])){
          $title[] = $user->field_primer_nombre['und'][0]['value'];
        }
        if(isset($user->field_segundo_nombre['und'][0]['value'])){
          $title[] = $user->field_segundo_nombre['und'][0]['value'];
        }
        if(isset($user->field_primer_apellido['und'][0]['value'])){
          $title[] = $user->field_primer_apellido['und'][0]['value'];
        }
        if(isset($user->field_segundo_apellido['und'][0]['value'])){
          $title[] = $user->field_segundo_apellido['und'][0]['value'];
        }               
        $current_row->title = implode(" ", $title);
      }
    }
  
    // Source ID
    $source_id = $current_row->id_de_directorio;

    // Categoria de Directorio
    if(isset($current_row->categoria) && !empty($current_row->categoria)){
      $current_row->categoria = get_categoria_directorio($current_row->categoria);
    }

    // Etiquetas de Directorio
    $tags = array();
    if(isset($current_row->palabras_clave) && !empty($current_row->palabras_clave)){
      $tags_items = explode(',', $current_row->palabras_clave);
      foreach ($tags_items as $key => $tag) {
        $item = trim($tag);
        if(!empty($item)){
          $tags[] = $item;
        }
      }
    }
    $current_row->etiquetas_directorio = $tags;

    // Estado de Publicación
    if(isset($current_row->estado_equivalencia) && !empty($current_row->estado_equivalencia)){
      $current_row->estado_equivalencia = get_estado_publicacion($current_row->estado_equivalencia);
    }

    // Status
    if(isset($current_row->estado) && !empty($current_row->estado)){
      $current_row->estado = strtoupper($current_row->estado);
      if(($current_row->estado == 'A') || ($current_row->estado == 'V')){
        $current_row->estado = 1;
      }elseif($current_row->estado == 'I'){
        $current_row->estado = 0;
      }
    }
    
    // Fecha de Creación
    if(isset($current_row->fecha_crea) && !empty($current_row->fecha_crea)){
      $current_row->fecha_crea = strtotime($current_row->fecha_crea);
    }

    // Fecha de Modificación
    if(isset($current_row->fecha_modifica) && !empty($current_row->fecha_modifica)){
      $current_row->fecha_modifica = strtotime($current_row->fecha_modifica);
    }

    return TRUE;
  }

  /**
   * Runs before an import starts.
   * Code to execute before first article row is imported
   */
  public function preImport() {
    parent::preImport();
  }

  /*
   * The Migration class complete() method is analogous to prepare(), but is called immediately after the complete
   * destination object is saved.
   */
   public function prepare(stdClass $entity, stdClass $current_row) {

   }

  /*
   * The Migration class complete() method is analogous to prepare(), but is called immediately after the complete
   * destination object is saved.
   */
   public function complete($entity, stdClass $current_row) {
   		$node = $entity;
   }

}
