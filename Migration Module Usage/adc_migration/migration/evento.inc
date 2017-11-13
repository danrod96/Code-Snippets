<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);

require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class EventoNodeMigration extends Migration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('CSV feed (pull) of Nodes tipo evento');

    // No mapear campo de contratante
    $fields = array(
      0  => array('id_evento', 'id_evento'),
      1  => array('idioma', 'idioma'),
      2  => array('categoria', 'categoria'),
      3  => array('nombre_categoria', 'nombre_categoria'),
      4  => array('tipo_evento', 'tipo_evento'),
      5  => array('nombre_tipo_evento', 'nombre_tipo_evento'),
      6  => array('nombre_evento', 'nombre_evento'),
      7  => array('especialidad', 'especialidad'),
      8  => array('descripcion', 'descripcion'),
      9  => array('pais', 'pais'),
      10  => array('departamento', 'departamento'),
      11  => array('ciudad', 'ciudad'),
      12  => array('direccion', 'direccion'),
      13 => array('fecha_inicio', 'fecha_inicio'),
      14 => array('fecha_fin', 'fecha_fin'),
      15 => array('telefono_1', 'telefono_1'),
      16 => array('telefono_2', 'telefono_2'),
      17 => array('celular', 'celular'),
      18 => array('fax', 'fax'),
      19 => array('correo_contacto', 'correo_contacto'),
      20 => array('url', 'url'),
      21 => array('observaciones', 'observaciones'),
      22 => array('codigo_estado', 'codigo_estado'),
      23 => array('nombre_estado', 'nombre_estado'),
      24 => array('estado', 'estado'),
      25 => array('id_usuario_creacion', 'id_usuario_creacion'),
      26 => array('fecha_creacion', 'fecha_creacion'),
      27 => array('id_usuario_modificacion', 'id_usuario_modificacion'),
      28 => array('fecha_modificacion', 'fecha_modificacion'),
    );

    //The Source of the import
    $path = 'public://migration/eventos-3.csv';

    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type contrato
    $this->destination = new MigrateDestinationNode('evento');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_evento' => array('type' => 'varchar', 'length' => 255, 'description' => 'Id evento', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());


    $this->addFieldMapping('field_id_anterior', 'id_evento');
    $this->addFieldMapping('title', 'nombre_evento');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('changed', 'changed');
    $this->addFieldMapping('publish_on', 'fecha_inicio');
    $this->addFieldMapping('unpublish_on', 'fecha_fin');
    $this->addFieldMapping('uid', 'id_usuario_creacion');
    $this->addFieldMapping('status', 'estado');
    $this->addFieldMapping('field_categoria_evento','categoria');
    $this->addFieldMapping('field_tipo_evento','tipo_evento');
    $this->addFieldMapping('body', 'descripcion');
    $this->addFieldMapping('field_direccion', 'direccion');
    $this->addFieldMapping('field_num_telefono', 'telefono_1');
    $this->addFieldMapping('field_num_celular', 'celular');
    $this->addFieldMapping('field_correo_electronico', 'correo_contacto');
    $this->addFieldMapping('field_pagina_web', 'url');

    // $this->addFieldMapping('runtime', 'seconds_to_run')->defaultValue(0);
    // $this->addFieldMapping('active')->defaultValue(1);// Unmapped destination fields
    // $this->addUnmigratedSources(array('active', 'runtime'));
    // $this->addFieldMapping('choice', 'choice');
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

    if(!valid_email_address($current_row->correo_contacto)){
      return FALSE;
    }

    // Status
    if(isset($current_row->estado) && !empty($current_row->estado)){
      $current_row->estado = strtoupper($current_row->estado);
      if(($current_row->estado == 'A') || ($current_row->estado == 'V')){
        $current_row->estado = 1;
      }else{
        $current_row->estado = 0;
      }
    }

    //Mapear categoria evento
    if(isset($current_row->categoria) && !empty($current_row->categoria)){
      $current_row->categoria = get_categoria_evento($current_row->categoria);
    }

    // Fecha de Creación
    if(!empty($current_row->fecha_creacion) && ($current_row->fecha_creacion != '#N/A')){
      $create_date = $current_row->fecha_creacion;
      $current_row->fecha_creacion = strtotime($create_date);
    }else{
      $current_row->fecha_creacion = time();
    }

    // Fecha de Modificación
    if(!empty($current_row->fecha_modificacion) && ($current_row->fecha_modificacion != '#N/A')){
      $edit_date = $current_row->fecha_modificacion;
      $current_row->fecha_modificacion = strtotime($edit_date);
    }else{
      $current_row->fecha_modificacion = time();
    }

    //Mapear Id del usuario (portal viejo) a UID usuario
    $current_row->id_usuario_creacion = get_uid_usuario($current_row->id_usuario_creacion);


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

    //Mapear categoria evento
    if(isset($current_row->categoria) && !empty($current_row->categoria)){
      $entity->field_categoria_evento['und'][0]['tid'] = $current_row->categoria;
    }

    //Guardar fechas inicio y fin evento
    if(isset($current_row->fecha_inicio) && !empty($current_row->fecha_inicio)){
      $entity->field_fecha_de_evento['und'][0]['value'] = strtotime($current_row->fecha_inicio);
      $entity->field_fecha_de_evento['und'][0]['timezone'] = "America/Bogota";
      $entity->field_fecha_de_evento['und'][0]['timezone_db'] = "UTC";
    }

    if(isset($current_row->fecha_fin) && !empty($current_row->fecha_fin)){
      $entity->field_fecha_de_evento['und'][0]['value2'] = strtotime($current_row->fecha_fin);
      $entity->field_fecha_de_evento['und'][0]['timezone'] = "America/Bogota";
      $entity->field_fecha_de_evento['und'][0]['timezone_db'] = "UTC";
    }

    //pais, depto y ciudad por si acaso
    if(isset($current_row->pais) && !empty($current_row->pais)){
      $tid_pais = get_tid_by_codigo_pais($current_row->pais);
        if(!empty($tid_pais)){
          $entity->field_ubicacion_desplegable['und'][0]['tid'] = $tid_pais;
        }
    }

    if(!empty($tid_pais) && $tid_pais == '196' && isset($current_row->departamento) && !empty($current_row->departamento)){
      $tid_depa = get_tid_by_codigo_departamento($current_row->departamento);
        if(!empty($tid_depa)){
          $entity->field_ubicacion_desplegable['und'][1]['tid'] = $tid_depa;
        }
    }

    if(!empty($tid_pais) && $tid_pais == '196' && !empty($tid_depa) && isset($current_row->ciudad) && !empty($current_row->ciudad)){
      $tid_ciudad = get_tid_by_codigo_ciudad($current_row->ciudad);
        if(!empty($tid_ciudad)){
          $entity->field_ubicacion_desplegable['und'][2]['tid'] = $tid_ciudad;
        }
    }

    //Mapear fecha creacion
    if(isset($current_row->fecha_creacion) && !empty($current_row->fecha_creacion)){
      $entity->created = $current_row->fecha_creacion;
    }

    //Mapear fecha modificacion
    if(isset($current_row->fecha_modificacion) && !empty($current_row->fecha_modificacion)){
      $entity->changed = $current_row->fecha_modificacion;
    }

    //Estado
    if(isset($current_row->estado) && !empty($current_row->estado)){
      $entity->status = $current_row->estado;
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
