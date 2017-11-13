<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);

/*
 * @file
 * The NoticiasToNodeMigration uses the  NOTICIAS.CSV files as source
 * and creates Drupal nodes of type 'article' as destination.
 */

require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class ArtesanosNodeMigration extends Migration {
	
  public $debug = false;
  public $ignore_file = false;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('CSV feed (pull) of User tipo Artesanos');

    $fields = array(
	    0 => array('id_del_artesano', 'ID DEL ARTESANO'),
      1 => array('id_del_usuario', 'ID DEL USUARIO'),
      2 => array('correo_electronico', 'CORREO ELECTRONICO'),
	    3 => array('primer_nombre', 'PRIMER NOMBRE'),
	    4 => array('segundo_nombre', 'SEGUNDO NOMBRE'),
	    5 => array('primer_apellido', 'PRIMER APELLIDO'),
      6 => array('segundo_apellido', 'SEGUNDO APELLIDO'),
      7 => array('artesano_genero', 'ARTESANO_GENERO F:FEMENINO M:MASCULINO'),
      8 => array('tipo_identificacion', 'TIPO DE IDENTIFICACION'),
      9 => array('numero_identificacion', 'NUMERO DE IDENTIFICACION'),
      10 => array('nivel_educativo', 'NIVEL EDUCATIVO'),
      11 => array('descripcion_nivel_educativo', 'DESCRIPCION NIVEL EDUCATIVO'),
      12 => array('codigo_pais', 'CODIGO DE PAIS'),
      14 => array('codigo_departamento', 'CODIGO DE DEPARTAMENTO'),
      16 => array('ciudad', 'CIUDAD'),
      18 => array('direccion', 'DIRECCION'),
      19 => array('numero_telefono', 'Número de Telefono'),
      20 => array('numero_fax', 'Número de Fax'),
      21 => array('numero_celular', 'Número de Celular'),
      22 => array('pagina_web_artesano', 'Pagina web del artesano'),
      23 => array('fecha_nacimiento', 'FECHA DE NACIMIENTO'),
      24 => array('tipo_artesano', 'TIPO DE ARTESANO'),
      25 => array('descripcion', 'DESCRIPCION'),
      28 => array('estado', 'ESTADO A-V:ACTIVO I:INACTIVO'),
      29 => array('usuario_creacion', 'USUARIO DE CREACION'),
      30 => array('fecha_creacion', 'FECHA DE CREACION'),
      31 => array('usuario_modificacion', 'USUARIO DE MODIFICACION'),
      32 => array('fecha_modificacion', 'FECHA DE MODIFICACION'),
    );

    //The Source of the import
    $path = DRUPAL_ROOT . '/sites/default/files/migration/artesanos.csv';
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type article
    $this->destination = new MigrateDestinationUser();

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_del_artesano' => array('type' => 'varchar', 'length' => 255, 'description' => 'ID DEL ARTESANO.', 'not null' => TRUE)), MigrateDestinationUser::getKeySchema());

    // Make the mappings
    $this->addFieldMapping('name', 'correo_electronico');
    $this->addFieldMapping('mail', 'correo_electronico');
    $this->addFieldMapping('pass', 'pass');
    
    $this->addFieldMapping('field_id_anterior', 'id_del_artesano');
    $this->addFieldMapping('field_id_del_usuario', 'id_del_usuario');
    $this->addFieldMapping('field_primer_nombre', 'primer_nombre');
    $this->addFieldMapping('field_segundo_nombre', 'segundo_nombre');
    $this->addFieldMapping('field_primer_apellido', 'primer_apellido');
    $this->addFieldMapping('field_segundo_apellido', 'segundo_apellido');
    $this->addFieldMapping('field_genero', 'artesano_genero');

    $this->addFieldMapping('field_direccion_origen', 'direccion');

    $this->addFieldMapping('field_tipo_documento', 'tipo_identificacion');
    $this->addFieldMapping('field_num_documento', 'numero_identificacion'); 

    $this->addFieldMapping('field_nivel_educativo', 'nivel_educativo'); 
    $this->addFieldMapping('field_nivel_educativo:source_type')->defaultValue('tid');

    $this->addFieldMapping('field_numero_de_fax', 'numero_fax');
    
    $this->addFieldMapping('field_codigo_pais', 'codigo_pais');

    $this->addFieldMapping('field_num_telefono', 'numero_telefono');
    $this->addFieldMapping('field_num_celular', 'numero_celular');
    $this->addFieldMapping('field_pagina_web', 'pagina_web_artesano');
    $this->addFieldMapping('field_fecha_nacimiento', 'fecha_nacimiento'); // Preprocesar
    $this->addFieldMapping('field_description_user', 'descripcion');
    $this->addFieldMapping('created', 'fecha_creacion'); // Preprocesar
    $this->addFieldMapping('changed', 'fecha_modificacion'); // Preprocesar

    $this->addFieldMapping('roles')->defaultValue(array(4 => 4));

    $this->addFieldMapping('field_tipo_perfil')->defaultValue(147);
    $this->addFieldMapping('field_tipo_perfil:source_type')->defaultValue('tid');

    $this->addFieldMapping('status', 'estado');
    
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
    $current_row->id_del_artesano = str_replace(".", "", $current_row->id_del_artesano);
    $current_row->id_del_artesano = str_replace(",", "", $current_row->id_del_artesano);

    //checkear si el id de artesano ya esta
    $result = db_query("SELECT entity_id FROM field_data_field_id_anterior WHERE bundle = 'user' AND field_id_anterior_value = :id_artesano LIMIT 1", array(":id_artesano" => $current_row->id_del_artesano))->fetchAssoc();

    if (!empty($result['entity_id'])){
      drupal_set_message(t("El usuario ya existe, el id del artesano es @id_artesano y su uid es: @uid", array("@uid" => $result['entity_id'], "@id_artesano" => $current_row->id_del_artesano)), "status");
      return FALSE;
    }

    //ID del usuario
    $current_row->id_del_usuario = str_replace(".", "", $current_row->id_del_usuario);
    $current_row->id_del_usuario = str_replace(",", "", $current_row->id_del_usuario);

    if(!valid_email_address($current_row->correo_electronico)){
      return FALSE;
    }else{
      $user = user_load_by_mail($current_row->correo_electronico);
      if($user != FALSE){
        return FALSE;
      }
    }

    // Source ID
    $source_id = $current_row->id_del_artesano;

    // Status
    if(isset($current_row->estado) && !empty($current_row->estado)){
      if(($current_row->estado == 'A') || ($current_row->estado == 'V')){
        $current_row->estado = 1;
      }elseif($current_row->estado == 'I'){
        $current_row->estado = 0;
      }
    }
    // password
    $current_row->pass = $current_row->numero_identificacion;
        
    // Tipo de identificación
    if(isset($current_row->tipo_identificacion) && !empty($current_row->tipo_identificacion)){
      $current_row->tipo_identificacion = get_tipo_identificacion($current_row->tipo_identificacion);
    }
    
    // Nivel educativo
    if(isset($current_row->nivel_educativo) && !empty($current_row->nivel_educativo)){
      $current_row->nivel_educativo = get_nivel_educativo($current_row->nivel_educativo);
    }
    
    // Tipo Perfil
    if(isset($current_row->tipo_artesano) && !empty($current_row->tipo_artesano)){
      $current_row->tipo_artesano = get_tipo_artesano($current_row->tipo_artesano);
    }

    // Fecha de Modificación
    if(isset($current_row->fecha_modificacion) && !empty($current_row->fecha_modificacion)){
      $current_row->fecha_modificacion = strtotime($current_row->fecha_modificacion);
    }

    // Fecha de Creación
    if(isset($current_row->fecha_creacion) && !empty($current_row->fecha_creacion)){
      $current_row->fecha_creacion = strtotime($current_row->fecha_creacion);
    }

    // Fecha de Nacimiento
    if(isset($current_row->fecha_nacimiento) && !empty($current_row->fecha_nacimiento)){
      $fecha_nacimiento = strtotime($current_row->fecha_nacimiento);
      $now = strtotime("now");
      if($fecha_nacimiento != FALSE && ($fecha_nacimiento < $now)){
        $current_row->fecha_nacimiento = $fecha_nacimiento;
      }else{
        $current_row->fecha_nacimiento = '';
      }
    }

    drupal_set_message(t("Creando usuario con id del artesano es @id_artesano", array("@id_artesano" => $current_row->id_del_artesano)), "status");

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
      $tids = array();
      // $tid_pais
      $tid_pais = '';
      if(isset($current_row->codigo_pais) && !empty($current_row->codigo_pais)){
        $tid_pais = get_tid_by_codigo_pais($current_row->codigo_pais);
        if(!empty($tid_pais)){
          $tids[] = array('tid' => $tid_pais);
        }
      }

      // $tid_departamento
      $tid_departamento = '';
      if( !empty($tid_pais) && $tid_pais == '196' && isset($current_row->codigo_departamento) && !empty($current_row->codigo_departamento) ){
        $tid_departamento = get_tid_by_codigo_departamento($current_row->codigo_departamento);
        if(!empty($tid_departamento)){
          $tids[] = array('tid' => $tid_departamento);
        }
      }

      // $tid_ciudad
      $tid_ciudad = '';
      if( !empty($tid_pais) && $tid_pais == '196' && !empty($tid_departamento) && isset($current_row->ciudad) && !empty($current_row->ciudad) ){
        $tid_ciudad = get_tid_by_codigo_ciudad($current_row->ciudad);
        if(!empty($tid_ciudad)){
          $tids[] = array('tid' => $tid_ciudad);
        }
      }

      if(!empty($tids)){
        // Set Ubicación.
        $entity->field_ubicacion_desplegable['und'] = $tids;
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
