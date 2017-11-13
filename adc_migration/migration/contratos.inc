<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);

/*
 * @file
 * The ContratossNodeMigration uses the  contratos.csv files as source
 * and creates Drupal nodes of type 'contrato' as destination.
 */

require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class ContratosNodeMigration extends Migration {
	
  public $debug = false;
  public $ignore_file = false;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('CSV feed (pull) of Nodes tipo contrato');
    // No mapear campo de contratante
    // 
    $fields = array(
	    0 => array('id_contrato', 'Id de Contrato'),
      1 => array('numero_contrato', 'Numero de contrato'),
	    //2 => array('proceso_contratacion_id', 'Proceso de contratacion id'),
	    2 => array('id_tipo_contrato', 'Id de tipo de contrato'),
	    3 => array('descripcion_tipo_contrato', 'Descripcion de tipo de contrato'),
      4 => array('id_tipo_proceso', 'id tipo de proceso'),
      //6 => array('descripcion_tipo_proceso', 'Descripcion tipo de proceso'),
      6 => array('fecha_firma', 'Fecha de Firma'),
      7 => array('pais_firma', 'Pais de firma'),
      9 => array('departamento_firma', 'Departamento de firma'),
      11 => array('ciudad_firma', 'Ciudad de firma'),
      13 => array('direccion_firma', 'Direccion de firma'),
      14 => array('moneda', 'Moneda COP=Pesos'),
      16 => array('valor_contrato', 'Valor del contrato'),
      17 => array('fecha_inicio', 'Fecha de inicio'),
      18 => array('fecha_final', 'Fecha final'),
      19 => array('objeto', 'Objeto'),
      20 => array('nombre_contratante', 'Nombre del Contratante'),
      21 => array('tipo_identificación_contratante', 'Tipo de identificación del contratante'),
      22 => array('Numero_identificacion_contratante', 'Numero de identificacion del contratante'),
      23 => array('nombre_contratista', 'Nombre del contratista'),
      24 => array('tipo_identificacion_contratista', 'Tipo de identificación del contratista'),
      25 => array('numero_identificacion_contratista', 'Numero de identificacion del contratista'),
      28 => array('archivo_anexo_id', 'Archivo anexo id'),
      //24 => array('contrato_extracto', 'contrato_extracto'),
      30 => array('estado_contrato', 'Estado del contrato A:Activo'),
      31 => array('usuario_crea', 'Usuario crea'),
      32 => array('fecha_crea', 'Fecha crea'),
      33 => array('usuario_modifica', 'Usuario modifica'),
      34 => array('fecha_modificacion', 'Fecha de modificacion'),
      34 => array('hoja_vida_archivo_adjunto', 'Hoja de vida archivo adjunto'),
      35 => array('nombre_archivo_fisico', 'Nombre del archivo fisico'),
      //32 => array('sedes', 'Sedes'),
      36 => array('dependencia', 'Dependencia'),
      37 => array('supervisor', 'Supervisor'),
    );

    //The Source of the import
    //$path = variable_get('contrato_csv_path', 'public://migration/contratos.csv');
    $path = DRUPAL_ROOT . '/sites/default/files/migration/contratos_nuevo.csv';
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type contrato
    $this->destination = new MigrateDestinationNode('contrato');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_contrato' => array('type' => 'varchar', 'length' => 255, 'description' => 'ID DEL CONTRATO.', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());

    
    // Field id de contrato <Texto>;
    $this->addFieldMapping('field_id_de_contrato', 'id_contrato');

    // Make the mappings
    // Field Titulo Clasificado <Texto>;
    $this->addFieldMapping('title', 'numero_contrato'); // Field Title 
    $this->addFieldMapping('field_identificador_contrato', 'numero_contrato'); // Field Title 
    $this->addFieldMapping('uid', 'author');
    // Descripción 
    $this->addFieldMapping('body', 'objeto');

    // Tipo de Contrato
    $this->addFieldMapping('field_tipo_contrato', 'id_tipo_contrato'); // Tipo de Contrato
    $this->addFieldMapping('field_tipo_contrato:source_type')->defaultValue('tid');

    // Tipo de proceso 
    $this->addFieldMapping('field_tipo_proceso', 'id_tipo_proceso');

    // Valor del contrato 
    $this->addFieldMapping('field_valor_contrato', 'valor_contrato');

    // Contratante
    $this->addFieldMapping('field_contratante', 'nombre_contratante');

    // Contratistas
    $this->addFieldMapping('field_contratista', 'nombre_contratista');

    // Tipo de documento Contratista
    $this->addFieldMapping('field_tipo_documento', 'tipo_identificacion_contratista');

    // Número de documento
    $this->addFieldMapping('field_num_doc_contratista', 'numero_identificacion_contratista');

    //Fecha de firma contrato
    $this->addFieldMapping('field_fecha_firma_contrato', 'fecha_firma');

    // Lugar de la firma
    $this->addFieldMapping('field_lugar_firma_contrato', 'direccion_firma');
    $this->addFieldMapping('field_lugar_firma_contrato:source_type')->defaultValue('tid');

    //Fecha de Ejecución Contrato
    $this->addFieldMapping('field_fecha_ejec_contrato', 'fecha_ejec_contrato');

    // PDF Contrato
    $this->addFieldMapping('field_archivo_contrato', 'nombre_archivo_fisico');
    $this->addFieldMapping('field_archivo_contrato:file_replace')->defaultValue(FILE_EXISTS_RENAME);
    $this->addFieldMapping('field_archivo_contrato:preserve_files')->defaultValue(TRUE);
    $this->addFieldMapping('field_archivo_contrato:destination_dir')->defaultValue('sites/default/files/pdf');
    // Hoja de vida
    $this->addFieldMapping('field_hv_relacionada', 'hoja_vida_archivo_adjunto');
   
    // Enlace Sistema Electrónico de Contratación Pública SECOP
    $this->addFieldMapping('field_enlace_secop', 'enlace_secop');

    // Supervisor
    $this->addFieldMapping('field_usuario_dependencia', 'supervisor');
    $this->addFieldMapping('field_usuario_dependencia:source_type')->defaultValue('target_id');

    // Supervisor
    $this->addFieldMapping('field_usuario_supervisor', 'supervisor');
    $this->addFieldMapping('field_usuario_supervisor:source_type')->defaultValue('target_id');

    // Dependencia
    $this->addFieldMapping('field_dependencia', 'dependencia');
    $this->addFieldMapping('field_dependencia:source_type')->defaultValue('tid');


    // Field Created Date
    $this->addFieldMapping('created', 'fecha_crea');

    // Field Changed Date
    $this->addFieldMapping('changed', 'fecha_modificacion');

    // Field Status
    $this->addFieldMapping('status', 'estado_contrato');

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
    $source_id = str_replace(".", "", $current_row->id_contrato);
    $source_id = str_replace(",", "", $current_row->id_contrato);

    $result = db_query("SELECT entity_id FROM field_data_field_id_de_contrato WHERE field_id_de_contrato_value = :contenido_id LIMIT 1", array(":contenido_id" => $current_row->id_contrato))->fetchAssoc();

    $result_tmp = db_query("SELECT entity_id FROM field_data_field_id_de_contrato WHERE field_id_de_contrato_value = :contenido_id LIMIT 1", array(":contenido_id" => $source_id))->fetchAssoc();

    $result_title = db_query("SELECT nid FROM node WHERE title = :num_interno LIMIT 1", array(":num_interno" => $current_row->numero_contrato))->fetchAssoc();

    if (!empty($result['entity_id']) || !empty($result_tmp['entity_id'])){
      drupal_set_message(t("Ya existia el siguiente id @external", array("@external" => $current_row->id_contrato)), "status");
      return FALSE;
    }

    // Descripción
    if(isset($current_row->objeto) && !empty($current_row->objeto)){
      $current_row->objeto = cleanHtmlEntities($current_row->objeto);
    }

    // Tipo de Contrato
    if(isset($current_row->id_tipo_contrato) && !empty($current_row->id_tipo_contrato)){
      //$current_row->id_tipo_contrato_raw = $current_row->id_tipo_contrato;
      $current_row->id_tipo_contrato = get_categoria_contrato($current_row->id_tipo_contrato);
    }

    // Tipo de proceso
    if(isset($current_row->id_tipo_proceso) && !empty($current_row->id_tipo_proceso)){
      //$current_row->descripcion_tipo_proceso_raw = $current_row->descripcion_tipo_proceso;
      $current_row->id_tipo_proceso = get_categoria_proceso($current_row->id_tipo_proceso);
    }
    
    // Valor Contrato
    if(isset($current_row->valor_contrato) && !empty($current_row->valor_contrato)){
      $current_row->valor_contrato =  str_replace(".", "", $current_row->valor_contrato);
      $current_row->valor_contrato =  str_replace(",", "", $current_row->valor_contrato);
    }

    // Contratante
    $current_row->nombre_contratante = 'ARTESANÍAS DE COLOMBIA NIT: 860007887';

    // Tipo de documento Contratista
    if(isset($current_row->tipo_identificacion_contratista) && !empty($current_row->tipo_identificacion_contratista)){
      $current_row->tipo_identificacion_contratista_raw = $current_row->tipo_identificacion_contratista;
      $current_row->tipo_identificacion_contratista = get_tipo_identificacion($current_row->tipo_identificacion_contratista);
    }
    
    // Fecha de Firma
    if(isset($current_row->fecha_firma) && !empty($current_row->fecha_firma)){
      $current_row->fecha_firma = strtotime($current_row->fecha_firma);
    }

    // Lugar de la firma
    if(isset($current_row->direccion_firma) && !empty($current_row->direccion_firma)){
      $current_row->direccion_firma = get_lugar_firma($current_row->direccion_firma);
    }

    // Fecha de Ejecución Contrato
    $from = '';
    if(isset($current_row->fecha_inicio) && !empty($current_row->fecha_inicio)){
      $current_row->fecha_inicio = strtotime($current_row->fecha_inicio);
      $from = date('Y-m-d H:i:s', $current_row->fecha_inicio);
    }
    $to = '';
    if(isset($current_row->fecha_final) && !empty($current_row->fecha_final)){
      $current_row->fecha_final = strtotime($current_row->fecha_final);
      $to = date('Y-m-d H:i:s', $current_row->fecha_final);
    }

    $current_row->fecha_ejec_contrato = drupal_json_encode(array(
      'from' => (string) $from,
      'to' => (string) $to,
    ));
    
    // PDF Contrato
    if( isset($current_row->nombre_archivo_fisico) && !empty($current_row->nombre_archivo_fisico) && !(strpos($current_row->nombre_archivo_fisico, 'N/A') !== false) ){
      $file = file_create_url('public://pdf/'.$current_row->nombre_archivo_fisico);
       //$file = 'http://www.esinergia.co/adc/ftp/Documentos/' . $current_row->nombre_archivo_fisico; 
       $current_row->nombre_archivo_fisico = $file;
    }else{
      $current_row->nombre_archivo_fisico = '';
    }

    // Hoja de vida
     if(isset($current_row->hoja_vida_archivo_adjunto) && !empty($current_row->hoja_vida_archivo_adjunto)){
      $current_row->hoja_vida_archivo_adjunto = get_hoja_vida_archivo_adjunto($current_row->hoja_vida_archivo_adjunto);
    }
    
    // Enlace Sistema Electrónico de Contratación Pública SECOP
    $current_row->enlace_secop = 'Artesanías de Colombia ademas publica toda la información de su contratación en el SECOP Sistema Electrónico de Contratación Pública. 
<br/>www.contratos.gov.co';
    
    // Dependencia
    if(isset($current_row->dependencia) && !empty($current_row->dependencia)){
      $current_row->dependencia = get_dependencia_contrato($current_row->dependencia);
    }

    if(isset($current_row->supervisor) && !empty($current_row->supervisor)){
     $current_row->supervisor = get_uid($current_row->supervisor);
    }
    
    // Status
    if(isset($current_row->estado_contrato) && !empty($current_row->estado_contrato)){
      $current_row->estado_contrato = strtoupper($current_row->estado_contrato);
      if(($current_row->estado_contrato == 'A') || ($current_row->estado_contrato == 'V')){
        $current_row->estado_contrato = 1;
      }else{
        $current_row->estado_contrato = 0;
      }
    }
    
    // Fecha de Creación
    if(isset($current_row->fecha_crea) && !empty($current_row->fecha_crea)){
      $current_row->fecha_crea = strtotime($current_row->fecha_crea);
    }

    // Fecha de Modificación
    if(isset($current_row->fecha_modificacion) && !empty($current_row->fecha_modificacion)){
      $current_row->fecha_modificacion = strtotime($current_row->fecha_modificacion);
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
