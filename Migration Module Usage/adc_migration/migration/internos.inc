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

class InternosNodeMigration extends Migration {
	
  public $debug = false;
  public $ignore_file = false;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('CSV feed (pull) of User tipo Internos');

    $fields = array(
	    0 => array('id_del_artesano', 'Id de funcionario'),
      1 => array('id_del_usuario', 'Usuario de funcionario'),
	    2 => array('primer_nombre', 'PRIMER NOMBRE'),
	    3 => array('segundo_nombre', 'SEGUNDO NOMBRE'),
	    4 => array('primer_apellido', 'PRIMER APELLIDO'),
      5 => array('segundo_apellido', 'SEGUNDO APELLIDO'),
      6 => array('tipo_identificacion', 'TIPO DE IDENTIFICACION'),
      7 => array('numero_identificacion', 'NUMERO DE IDENTIFICACION'),
      8 => array('numero_telefono', 'Número de Telefono'),
      9 => array('numero_celular', 'Número de Celular'),
      10 => array('fecha_nacimiento', 'FECHA DE NACIMIENTO'),
      11 => array('dependencia', 'Dependencia'),
      12 => array('cargo', 'cargo'),
      13 => array('vinculacion', 'vinculacion'),
      14 => array('estado', 'ESTADO A-V:ACTIVO I:INACTIVO'),
      15 => array('usuario_creacion', 'USUARIO DE CREACION'),
      16 => array('fecha_creacion', 'FECHA DE CREACION'),
      17 => array('usuario_modificacion', 'USUARIO DE MODIFICACION'),
      18 => array('fecha_modificacion', 'FECHA DE MODIFICACION'),
      19 => array('pais', 'pais'),
      20 => array('extension', 'Extensión'),
      21 => array('telefono_carea', 'FUNCIONARIO_TELEFONO_CAREA'),
      22 => array('correo_electronico', 'CORREO ELECTRONICO'),
    );

    //The Source of the import
    $path = variable_get('internos_csv_path', 'public://migration/internos.csv');
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
    $this->addFieldMapping('field_codigo_pais','pais');
    $this->addFieldMapping('field_codigo_area','telefono_carea');

    $this->addFieldMapping('field_tipo_documento', 'tipo_identificacion');
    $this->addFieldMapping('field_num_documento', 'numero_identificacion'); 

    $this->addFieldMapping('field_dependencia', 'dependencia');
    $this->addFieldMapping('field_dependencia:source_type')->defaultValue('tid');

    $this->addFieldMapping('field_cargo', 'cargo');
    $this->addFieldMapping('field_cargo:source_type')->defaultValue('tid');

    $this->addFieldMapping('field_num_telefono', 'numero_telefono');
    $this->addFieldMapping('field_ext_telefono', 'extension');
    $this->addFieldMapping('field_num_celular', 'numero_celular');
    $this->addFieldMapping('field_fecha_nacimiento', 'fecha_nacimiento'); // Preprocesar
    $this->addFieldMapping('created', 'fecha_creacion'); // Preprocesar
    $this->addFieldMapping('changed', 'fecha_modificacion'); // Preprocesar

    $this->addFieldMapping('roles')->defaultValue(array(5 => 5));

//    $this->addFieldMapping('field_tipo_perfil')->defaultValue(147);
//    $this->addFieldMapping('field_tipo_perfil:source_type')->defaultValue('tid');

    $this->addFieldMapping('status', 'estado');
    
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
    
    // dependencia
    if(isset($current_row->dependencia) && !empty($current_row->dependencia)){
      $current_row->dependencia = get_dependencia($current_row->dependencia);
    }
    // cargo
    if(isset($current_row->cargo) && !empty($current_row->cargo)){
      $current_row->cargo = get_cargo($current_row->cargo);
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
