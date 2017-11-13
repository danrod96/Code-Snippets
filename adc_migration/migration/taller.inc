<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);

require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class TallerNodeMigration extends Migration {

  public $debug = false;
  public $ignore_file = false;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('CSV feed (pull) of User tipo Taller');

    $fields = array(
      0 => array('id_taller', 'id_taller'),
      1 => array('uid_admin_taller', 'usuario_administrador_taller'),
      2 => array('tipo_identificacion_taller', 'tipo_identificacion'),
      3 => array('identificacion_taller', 'identificacion'),
      4 => array('nombre_taller', 'nombre_taller'),
      5 => array('desc_taller', 'descripcion_taller'),
      6 => array('pais_taller', 'pais'),
      7 => array('nombre_pais_taller', 'nombre_pais'),
      8 => array('departamento_taller', 'departamento'),
      9 => array('nombre_departamento_taller', 'nombre_departamento'),
      10 => array('id_ciudad_taller', 'ciudad'),
      11 => array('nombre_ciudad_taller', 'nombre_ciudad'),
      12 => array('direccion_taller', 'direccion'),
      13 => array('telefono_taller', 'telefono'),
      14 => array('celular_taller', 'celular'),
      15 => array('fax_taller', 'fax'),
      16 => array('mail_taller', 'correo_electronico'),
      17 => array('url_taller', 'url'),
      18 => array('estado_wrf_taller', 'estado_galeria_taller'),
      19 => array('desc_wrf_estado_taller', 'descripcion_estado_galeria_taller'),
      20 => array('estado_taller', 'estado'),
      21 => array('usuario_crea_taller', 'usuario_creacion'),
      22 => array('fecha_crea_taller', 'fecha_creacion'),
      23 => array('usuario_modifica_taller', 'usuario_modifica'),
      24 => array('fecha_modifica_taller', 'fecha_modifica'),
    );

    //The Source of the import
    $path = DRUPAL_ROOT . '/sites/default/files/migration/talleres.csv';
    
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type directorio
    $this->destination = new MigrateDestinationNode('taller');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_taller' => array('type' => 'varchar', 'length' => 255, 'description' => 'ID DEL TALLER', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());

    $this->addFieldMapping('field_id_anterior_taller', 'id_taller');
    $this->addFieldMapping('title', 'nombre_taller');
    $this->addFieldMapping('body', 'desc_taller');
    $this->addFieldMapping('body:summary', 'body_summary');
    $this->addFieldMapping('body:format', 'body_format');
    $this->addFieldMapping('field_dpto_ciudad', 'pais_taller')->defaultValue('tid');
    $this->addFieldMapping('field_direccion', 'direccion_taller');
    $this->addFieldMapping('field_correo_electronico', 'mail_taller');
    $this->addFieldMapping('field_pagina_web', 'url_taller');
    $this->addFieldMapping('field_num_celular', 'celular_taller');
    $this->addFieldMapping('field_num_telefono', 'telefono_taller');
    $this->addFieldMapping('created', 'fecha_crea_taller');
    // Field Changed Date
    $this->addFieldMapping('changed', 'fecha_modifica_taller');
    // Field Status
    $this->addFieldMapping('status', 'estado_taller');
    $this->addFieldMapping('uid', 'usuario_crea_taller');

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
    $source_id = $current_row->id_taller;

    //Mapear Id del usuario artesano (portal viejo) a UID usuario artesano
    //usuario_admin_taller y usuario_crea_taller deberian ser el mismo
    $current_row->usuario_crea_taller = get_uid_usuario($current_row->uid_admin_taller);

    // Estado Taller
    if(isset($current_row->estado_taller) && !empty($current_row->estado_taller)){
      if(($current_row->estado_taller == 'A') || ($current_row->estado_taller == 'V')){
        $current_row->estado_taller = 1;
      }elseif($current_row->estado_taller == 'I'){
        $current_row->estado_taller = 0;
      }
    }

    // Body
    $current_row->body_format = "full_html"; // Filtered HTML
    $current_row->body_summary = $current_row->body_summary; // Body Summary
    $current_row->body_formatted = '';
    $current_row->body = '';

    if(!empty($current_row->body_text)){
      $current_row->body_formatted = cleanHtmlEntities($current_row->body_text);
      $current_row->body = $current_row->body_formatted;
    }

    //Fecha creacion taller
    if(!empty($current_row->fecha_crea_taller) && ($current_row->fecha_crea_taller != '#N/A')){
      $create_date = $current_row->fecha_crea_taller;
      $current_row->fecha_crea_taller = strtotime($create_date);
    }else{
      $current_row->fecha_crea_taller = time();
    }

    // Fecha de Modificación producto taller
    if(!empty($current_row->fecha_modifica_taller) && ($current_row->fecha_modifica_taller != '#N/A')){
      $edit_date = $current_row->fecha_modifica_taller;
      $current_row->fecha_modifica_taller = strtotime($edit_date);
    }else{
      $current_row->fecha_modifica_taller = time();
    }

    //Almacenar estado workflow
    if(isset($current_row->estado_wrf_taller) && !empty($current_row->estado_wrf_taller)){
      $retval = get_estado_publicacion_taller($current_row->estado_wrf_taller);

      if ($retval){
          $current_row->estado_wrf_taller = $retval;
      } else {
          $current_row->estado_wrf_taller = get_estado_publicacion_taller('N');
      }
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

    //Mapear fecha creacion
    if(isset($current_row->fecha_crea_taller) && !empty($current_row->fecha_crea_taller)){
      $entity->created = $current_row->fecha_crea_taller;
    }

    //Mapear fecha modificacion
    if(isset($current_row->fecha_modifica_taller) && !empty($current_row->fecha_modifica_taller)){
      $entity->changed = $current_row->fecha_modifica_taller;
    }

    //Estado
    if(isset($current_row->estado_taller) && !empty($current_row->estado_taller)){
      $entity->status = $current_row->estado_taller;
    }

    $tids = array();
    // $tid_pais
    $tid_pais = '';
    if(isset($current_row->pais_taller) && !empty($current_row->pais_taller)){
      $tid_pais = get_tid_by_codigo_pais($current_row->pais_taller);
      if(!empty($tid_pais)){
        $tids[] = array('tid' => $tid_pais);
      }
    }

    // $tid_departamento
    $tid_departamento = '';
    if( !empty($tid_pais) && $tid_pais == '196' && isset($current_row->departamento_taller) && !empty($current_row->departamento_taller) ){
      $tid_departamento = get_tid_by_codigo_departamento($current_row->departamento_taller);
      if(!empty($tid_departamento)){
        $tids[] = array('tid' => $tid_departamento);
      }
    }

    // $tid_ciudad
    $tid_ciudad = '';
    if( !empty($tid_pais) && $tid_pais == '196' && !empty($tid_departamento) && isset($current_row->id_ciudad_taller) && !empty($current_row->id_ciudad_taller) ){
      $tid_ciudad = get_tid_by_codigo_ciudad($current_row->id_ciudad_taller);
      if(!empty($tid_ciudad)){
        $tids[] = array('tid' => $tid_ciudad);
      }
    }

    if(!empty($tid_ciudad)){
      // Set Ubicación.
      $entity->field_dpto_ciudad['und'][0]['tid'] = $tid_ciudad;
    } else if (!empty($tid_departamento) && empty($tid_ciudad)){
      $entity->field_dpto_ciudad['und'][0]['tid'] = $tid_departamento;
    } else if (!empty($tid_pais) && empty($tid_departamento) && empty($tid_ciudad)){
      $entity->field_dpto_ciudad['und'][0]['tid'] = $tid_pais;
    }

    //Workflow
    if(isset($current_row->estado_wrf_taller) && !empty($current_row->estado_wrf_taller)){
      $entity->field_estado_taller['und'][0]['value'] = $current_row->estado_wrf_taller;
    }

    //Estado
    if(isset($current_row->estado_taller) && !empty($current_row->estado_taller)){
      $entity->status = $current_row->estado_taller;
    }

   }

  /*
   * The Migration class complete() method is analogous to prepare(), but is called immediately after the complete
   * destination object is saved.
   */
   public function complete($entity, stdClass $current_row) {
   		$node = $entity;
   }

}
