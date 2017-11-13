<?php

// +======================+
// | Cargar Imagenes      |
// +======================+
/*
*funcion load_images
*
*Carga las imagenes validando si se hace desde un servidor externo por ftp o si se cargan desde el mismo servidor
*
*/
function load_images($imagenes, $type = "multiple"){
	$images_files = array();
    $arrayImages = $imagenes;
    foreach ($arrayImages as $key => $imagen) {
			$url_image = file_create_url("public://imagenes/".$imagen);
			try {
				$image = file_get_contents($url_image); // string
				$file = file_save_data($image, "public://imagenes/".$imagen, FILE_EXISTS_REPLACE);
  			$images_files[] = $file->uri;
  			if($type == "one"){
  				return array('und' => array((array)$file));
  			}

			} catch (Exception $e) {
				dpm($e->getMessage());
			}

    }
  return $images_files;
}


/*
* get_estado_de_publicacion_clasificado
*/
function get_estado_de_publicacion_clasificado($id = ''){
	$mapping = array('N'=>30, 'E'=>52, 'P'=>31, 'R'=>32, 'V'=>51);

    if (isset($mapping[$id])){
       return $mapping[$id];
    }
    else{
       return false;
    }

}

/*
* Get Estado de Publicación
*/
function get_estado_publicacion($id){
	$mapping = array('N'=>26, 'E'=>55, 'P'=>27, 'A'=>28);

    if (isset($mapping[$id])){
       return $mapping[$id];
    }
    else{
       return false;
    }
}

/**
 * Get Estado Publicacion Producto Taller
 */
function get_estado_publicacion_producto_taller($id){
	$mapping = array('N' => 38, 'P' => 39, 'R' => 40, 'E' => 54);

    if (isset($mapping[$id])){
       return $mapping[$id];
    }
    else{
       return false;
    }
}

/**
 * Get Estado Publicacion Taller
 */
function get_estado_publicacion_taller($id){
	$mapping = array('N' => 34, 'R' => 36, 'P' => 35, 'E' => 53);

    if (isset($mapping[$id])){
       return $mapping[$id];
    }
    else{
       return false;
    }
}

/* Get Estado Contactenos */
function get_estado_publicacion_contactenos($id){
	$mapping = array('N' => 22, 'A' => 23, 'S' => 24, 'R' => 48);

    if (isset($mapping[$id])){
       return $mapping[$id];
    }
    else{
       return false;
    }
}

/* Get Estado Eventos */
function get_estado_publicacion_eventos($id){
	//N -> NUEVA
	//A -> APROBADA
	//P -> PUBLICADA
	//E -> EDICION
	//V -> VENCIDA
	//R -> NO APROBADA
	$mapping = array('N' => 42, 'A' => 43, 'P' => 43, 'E' => 50, 'V' => 49, 'R' => 44);

    if (isset($mapping[$id])){
       return $mapping[$id];
    }
    else{
       return false;
    }
}

/*
* Get Categoria Proceso
*/
function get_categoria_proceso($source_categoria){
	$destination_categoria_tid = '';
	if(!empty($source_categoria)){
		//$source_categoria = trim(strtolower($source_categoria));
		switch ($source_categoria) {
		    //case 'invitacion abierta':
		    case 'A':
		        $destination_categoria_tid = 'abierta';
		        break;
		    //case 'invitacion cerrada':
		    case 'C':
		        $destination_categoria_tid = 'cerrada';
		        break;
		    //case 'directa':
		    case 'D':
		        $destination_categoria_tid = 'directa';
		        break;
		    default:
		    	$destination_categoria_tid = '';
		}
	}
	return $destination_categoria_tid;
}

/*
* Get Categoria Contrato
*/
function get_categoria_contrato($source_categoria){
	$destination_categoria_tid = '';
	if(!empty($source_categoria)){
		//$source_categoria = trim(strtolower($source_categoria));
		switch ($source_categoria) {
		    //case 'arrendamiento':
		    case 'I':
		        $destination_categoria_tid = '42';
		        break;
		    //case 'asesoria':
		    case 'E':
		        $destination_categoria_tid = '43';
		        break;
		    //case 'compra venta':
		    case 'B':
		        $destination_categoria_tid = '44';
		        break;
		    //case 'consignacion de mercancias':
		    case 'J':
		        $destination_categoria_tid = '45';
		        break;
		    //case 'consultoria':
		    case 'F':
		        $destination_categoria_tid = '46';
		        break;
		    //case 'convenios':
		    case 'H':
		        $destination_categoria_tid = '47';
		        break;
		    //case 'mandatos':
		    case 'G':
		        $destination_categoria_tid = '48';
		        break;
		    //case 'obra':
		    case 'D':
		        $destination_categoria_tid = '49';
		        break;
		    //case 'prestacion de servicios':
		    case 'A':
		        $destination_categoria_tid = '50';
		        break;
		    // case 'servicios':
		    case 'S':
		        $destination_categoria_tid = '51';
		        break;
		    //case 'suministro':
		    case 'C':
		        $destination_categoria_tid = '52';
		        break;
		    default:
		    	$destination_categoria_tid = '';
		}
	}
	return $destination_categoria_tid;
}

/*
* Get categoria de clasificado
*/
function get_categoria_clasificado($id){

	$mapping = array(1=>1544,2=>1545,3=>1547,4=>1546,5=>1550,6=>1549,7=>1548);
    if (isset($mapping[$id])){
       return $mapping[$id];
    }
    else{
       return false;
    }
}

/*
* Get categoria de directorio
*/
function get_categoria_directorio($id){
    $mapping = array(8=>93,10=>91,1=>95,2=>96,9=>92,5=>97,3=>98,6=>99,7=>100,12=>101);
    if (isset($mapping[$id])){
       return $mapping[$id];
    }
    else{
       return false;
    }
}

/*
* Get categoria de evento
*/
function get_categoria_evento($id){
    $mapping = array(2=>1806,4=>1804,5=>1809,6=>1805,7=>1808,8=>1810,9=>1807,10=>13281);
    if (isset($mapping[$id])){
       return $mapping[$id];
    }
    else{
       return false;
    }
}

/*
* Load term that contains name
*/
function taxonomy_term_contains_name($name, $vid, $parent = 0) {
  $query = db_select('taxonomy_term_data', 't');
  $query->join('taxonomy_term_hierarchy', 'h', 'h.tid = t.tid');
  $result = $query->fields('t')
  				  ->fields('h', array('parent'))
  				  ->condition('t.vid', $vid)
  				  ->condition('t.name', '%' . db_like($name) . '%', 'LIKE')
  				  ->condition('h.parent', $parent)
  				  ->orderBy('t.weight')
  				  ->orderBy('t.name')
  				  ->execute();
  $term = array();
  while($record = $result->fetchAssoc()) {
   	$term[] = $record;
  }
  return $term;
}

/*
* get tid by codigo pais
*/
function get_tid_by_codigo_pais($codigo_pais = ''){
	$tid = '';
	if(!empty($codigo_pais)){
		$vid = '21';
		$query = new EntityFieldQuery();
		$result = $query
		  ->entityCondition('entity_type', 'taxonomy_term')
		  ->propertyCondition('vid', (int) $vid, '=')
		  -> fieldCondition('field_codigo_de_pais', 'value', $codigo_pais, '=')
		  ->execute();
		if (!empty($result['taxonomy_term'])) {
		   $taxonomy_term = array_keys($result['taxonomy_term']);
		   $tid = $taxonomy_term[0];
		}
	}
	return $tid;
}

/*
* get tid by codigo departamento
*/
function  get_tid_by_codigo_departamento($codigo_departamento = ''){
	$tid = '';
	if(!empty($codigo_departamento)){
		//$codigo_departamento = number_format($codigo_departamento);
		$codigo_departamento = str_replace(",", ".", $codigo_departamento);
		$vid = '21';
		$query = new EntityFieldQuery();
		$result = $query
		  ->entityCondition('entity_type', 'taxonomy_term')
		  ->propertyCondition('vid', (int) $vid, '=')
		  -> fieldCondition('field_codigo_de_departamento', 'value', $codigo_departamento, '=')
		  ->execute();
		if (!empty($result['taxonomy_term'])) {
		   $taxonomy_term = array_keys($result['taxonomy_term']);
		   $tid = $taxonomy_term[0];
		}
	}
	return $tid;
}

/*
* get tid by codigo ciudad
*/
function  get_tid_by_codigo_ciudad($ciudad = ''){
	$tid = '';
	if(!empty($ciudad)){
		//$ciudad = number_format($ciudad);
		$ciudad = str_replace(",", ".", $ciudad);
		$vid = '21';
		$query = new EntityFieldQuery();
		$result = $query
		  ->entityCondition('entity_type', 'taxonomy_term')
		  ->propertyCondition('vid', (int) $vid, '=')
		  -> fieldCondition('field_codigo_de_ciudad', 'value', $ciudad, '=')
		  ->execute();
		if (!empty($result['taxonomy_term'])) {
		   $taxonomy_term = array_keys($result['taxonomy_term']);
		   $tid = $taxonomy_term[0];
		}
	}
	return $tid;
}

/*
* Get Map for Tipo de Artesano
*/
function get_tipo_artesano($tipo_artesano = ''){
	$destination_tipo_artesano = '';
	if(!empty($tipo_artesano)){
		$source_tipo_artesano = $tipo_artesano;
		switch ($source_tipo_artesano) {
		    case '1': // Aprendiz
		        $destination_tipo_artesano = '13181';
		        break;
		    case '2': // Instructor
		        $destination_tipo_artesano = '13182';
		        break;
		    case '3': // Maestro artesano
		        $destination_tipo_artesano = '13183';
		        break;
		    case '4': // Experto en Arte Manual
		        $destination_tipo_artesano = '13184';
		    default:
		    	$destination_tipo_artesano = '';
		}
	}
	return $destination_tipo_artesano;
}

function get_dependencia($iddep){
	$mapping_dependence = array('1'=>1849,'2'=>1850,'3'=>1851,'4'=>1852,'5'=>1853,'6'=>1854,'7'=>1855,'8'=>1856,'9'=>1857,'10'=>1858,'12'=>1860,'13'=>1861,'14'=>1862,'15'=>1863,'16'=>1864,'17'=>1865,'18'=>1866,'19'=>1867,'20'=>1868,'21'=>1869,'22'=>1870,'23'=>1871,'24'=>1872,'25'=>1873,'26'=>1874);

    if (isset($mapping_dependence[$iddep])){
    	return $mapping_dependence[$iddep];
    }
    else{
    	return FALSE;
    }
}
function get_tema_faq($idtema){
	$mapping_temas = array(2=>14162,3=>14163,4=>14164,5=>14165,6=>14166,7=>14167,8=>14168,9=>14169,10=>14170,1=>14171,11=>14172,62=>14173);

    if (isset($mapping_temas[$idtema])){
    	return $mapping_temas[$idtema];
    }
    else{
    	return FALSE;
    }
}

function get_cargo($idcar){
	$mapping_cargo = array('2'=>32,'3'=>33,'4'=>34,'5'=>35,'6'=>36,'7'=>37,'8'=>38,'9'=>13296,'10'=>40,'11'=>41,'12'=>1876,'13'=>1877);

    if (isset($mapping_cargo[$idcar])){
    	return $mapping_cargo[$idcar];
    }
    else{
    	return FALSE;
    }
}

/*
* Load term that contains name
*/
function get_departamento_tid_by_codigo_departamento($codigo_departamento = '', $vid) {
	$query = new EntityFieldQuery();
	$results = $query
	->entityCondition('entity_type', 'taxonomy_term')
	->propertyCondition('vid', (int) $vid, '=')
	-> fieldCondition('field_codigo_de_departamento', 'value', $codigo_departamento, '=')
	->execute();

	if (!empty($results['taxonomy_term'])) {
	   $taxonomy_term = array_keys($results['taxonomy_term']);
	   return $taxonomy_term[0];
	}

	return FALSE;
}

/*
* Load node by Contenido Id
*/
function node_load_by_contenido_id($contenido_id = ""){
	if(empty($contenido_id)){
		return FALSE;
	}
	// Try Search on Noticias.
	 $result = db_select('migrate_map_noticias', 'n')
	    ->fields('n')
	    ->condition('sourceid1', $contenido_id, '=')
	    ->execute()
	    ->fetchAssoc();
	if(isset($result['destid1'])){
		return node_load($result['destid1']);
	}
    // Try Search on Publicaiones.
    $result = db_select('migrate_map_publicaciones', 'n')
	    ->fields('n')
	    ->condition('sourceid1', $contenido_id, '=')
	    ->execute()
	    ->fetchAssoc();
	if(isset($result['destid1'])){
		return node_load($result['destid1']);
	}
	return FALSE;
}

/*
* Remove special html entities like: &nbsp;
*/
function cleanHtmlEntities2($str = ''){
	$html_entity_decode = htmlentities($str, ENT_NOQUOTES);
	$html_entity_decode = html_entity_decode($html_entity_decode);
	$html_entity_decode = html_entity_decode($html_entity_decode);
	$html_entity_decode = html_entity_decode($html_entity_decode);
	$html_entity_decode = str_replace('&nbsp;', ' ', $html_entity_decode);
	$html_entity_decode = str_replace('‡;', 'á', $html_entity_decode);
	$html_entity_decode = str_replace('Ž', 'é', $html_entity_decode);
	$html_entity_decode = str_replace('’', 'í', $html_entity_decode);
	$html_entity_decode = str_replace('—n', 'ón', $html_entity_decode);
	$html_entity_decode = str_replace('œ', 'ú', $html_entity_decode);
	$html_entity_decode = str_replace('ƒ', 'É', $html_entity_decode);
	$html_entity_decode = str_replace('–a', 'ña', $html_entity_decode);

	return $html_entity_decode;
}

/*
* Remove special html entities like: &nbsp;
*/
function cleanHtmlEntities($str = ''){
	$html_entity_decode = htmlspecialchars_decode($str);
	$html_entity_decode = str_replace('&nbsp;', ' ', $html_entity_decode);
	return $html_entity_decode;
}

/*
*  Get UID from last UID
*  Nota: ID anterior: ID artesano
*/
function get_uid($old_uid = ""){
	$uid = 1;
	if($old_uid){
	  $old_uid = str_replace(".", "", $old_uid);
	  $old_uid = str_replace(",", "", $old_uid);
	  //create by
	  $query = db_select('field_data_field_id_anterior', 'n')
	    ->condition('field_id_anterior_value', $old_uid,'=')
	    ->fields('n', array('entity_id'))
	    ->range(0,1)
	    ->execute()
	  ->fetchAssoc();
	  $uid =(!empty($query['entity_id'])) ? $query['entity_id']:1;
	}
	return $uid;
}

/*
* Mapeo UID con base al id del usuario
*/
function get_uid_usuario($old_uid = ""){
	$uid = 1;
	if($old_uid){
	  $old_uid = str_replace(".", "", $old_uid);
	  $old_uid = str_replace(",", "", $old_uid);
	  //create by
	  $query = db_select('field_data_field_id_del_usuario', 'n')
	    ->condition('field_id_del_usuario_value', $old_uid,'=')
	    ->fields('n', array('entity_id'))
	    ->range(0,1)
	    ->execute()
	  ->fetchAssoc();
	  $uid =(!empty($query['entity_id'])) ? $query['entity_id']:1;
	}
	return $uid;
}

/*
* Node status
*/
function get_status($status_type = ''){
	$allowed_values =  array('A', 'E', 'V', 'I');
	if(!empty($status_type)){
		 $status_type = strtoupper($status_type);
		 if(in_array($status_type, $allowed_values)){
		 	return (in_array($status_type, array('A','V','E'))) ? 1: 0;
		 }else{
		 	return 0;
		 }
	}else{
		return 0;
	}
}

/*
*  Mapeo de Ids
*/
function _menu_item($index){
	$index = trim($index);
	$menu_items =  array(
		'2051' => '104',
		'2050' => '105',
		'2222' => '106',
		'2321' => '107',
		'2044' => '108',
		'2042' => '110',
		'3182' => '111',
		'2041' => '111',
		'2622' => '112',
		'2942' => '113',
		'2722' => '115',
		'2241' => '116',
		'2261' => '117',
		'2045' => '118',
		'2301' => '118',
		'3023' => '120',
		'1610' => '121',
		'2521' => '122',
		'1614' => '123',
		'1615' => '124',
		'2782' => '125',
		'3042' => '126',
		'1627' => '127',
		'1515' => '129',
		'1645' => '130',
		'2082' => '131',
		'3002' => '132',
		'1531' => '135',
		'1532' => '134',
		'2842' => '136',
		'1660' => '137',
		'3066' => '12819',
		'3150' => '12820',
		'3153' => '12821',
		'3155' => '12822',
		'3147' => '12823',
		'3082' => '12824',
		'3064' => '145',
		'3148' => '12825',
		'3146' => '12826',
		'3122' => '12827',
		'3149' => '12828',
		'3123' => '12829',
		'3156' => '12830',
		'3152' => '12831',
		'3154' => '12832',
		'3125' => '12833',
		'3151' => '12834',
		'3145' => '12835',
		'3102' => '12836',
		'3162' => '12837',
		'3124' => '12838',
		'3065' => '146'
	);
	return (isset($menu_items[$index]))? $menu_items[$index]: FALSE;
}

/*
* Get Dependencia Contrato
*/
function get_dependencia_contrato($source){
	$destination = '';
	if(!empty($source)){
		$source = strtolower($source);
		switch ($source) {
		    case 'subgerencia de desarrollo':
		        $destination = '1853';
		        break;
		    case 'subgerencia comercial':
		        $destination = '1871';
		        break;
		    case 'oficina asesora de planeación e información':
		        $destination = '1862';
		        break;
		    case 'gerencia general':
		        $destination = '1860';
		        break;
		    case 'subgerencia admininistrativa y financiera':
		        $destination = '1870';
		        break;
		    case 'subgerencia de desarrollo':
		        $destination = '1853';
		        break;
		    case 'subgerencia administrativa y financiera':
		        $destination = '1870';
		        break;
		    default:
		    	$destination = '';
		}
	}
	return $destination;
}

/*
* Get Hoja Vida Archivo Adjunto
*/
function get_hoja_vida_archivo_adjunto($hvid = ''){
	$hvnid = '';
	if(!empty($hvid)){
		$query = new EntityFieldQuery();
		$query->entityCondition('entity_type', 'node')
		  ->entityCondition('bundle', 'hoja_de_vida')
		  ->fieldCondition('field_id_de_hv', 'value', $hvid, '=')
		  ->range(0, 1);
		$result = $query->execute();
		if (isset($result['node'])) {
		  $news_items_nids = array_keys($result['node']);
		  if(isset($news_items_nids[0])){
		  	$hvnid = $news_items_nids[0];
		  }
		}
	}
	return $hvnid;
}

/*
* Get Lugar Firma
*/
function get_lugar_firma($source_tipo_identificacion){

	$destination_tipo_identificacion = '';
	if(!empty($source_tipo_identificacion)){
		$source_tipo_identificacion = str_replace("  ", " ", $source_tipo_identificacion);
		$search = array("Carrera 2", "Cra. 2", "Cra 2", "Cra.2", "Calle 74", "Pasto", "SAF", "Urbanizacion");
		$string_search = _get_lugar_firma_string ($source_tipo_identificacion, $search);
		if (!empty($string_search)) {
			$efq = new EntityFieldQuery();
			$result = $efq->entityCondition('entity_type', 'taxonomy_term')
				->propertyCondition('vid', 8)
				->fieldCondition('field_direccion_sede', 'value', $string_search, 'STARTS_WITH')
			->execute();
			$result = array_keys($result['taxonomy_term']);
			if (count($result) > 0 ) {
				return $result[0];
			}
		}
	}
	return $destination_tipo_identificacion;
}

function _get_lugar_firma_string($source_tipo_identificacion, $search){
	foreach ($search as $key => $value) {
		$pos = strpos($source_tipo_identificacion, $value);
		if ($pos !== false && ($key < 4) ) {
			$string_search = "Cra. 2";
			return $string_search;
		}elseif($pos !== false){

			$string_search = $value;
			return $string_search;
		}else{
			$string_search = "";
		}
	}
}

/*
* Mapeo de tipo de identificación
*/
function get_tipo_identificacion($source_tipo_identificacion = ''){
	$destination_tipo_identificacion = '';
	if(!empty($source_tipo_identificacion)){
		$source_tipo_identificacion = strtolower($source_tipo_identificacion);
		switch ($source_tipo_identificacion) {
		    case 'c.c':
		        $destination_tipo_identificacion = 'cc';
		        break;
		    case 'c.e':
		        $destination_tipo_identificacion = 'ce';
		        break;
		    case 'nit':
		        $destination_tipo_identificacion = 'nit';
		        break;
		    case 'pte':
		        $destination_tipo_identificacion = 'pasaporte';
		    case 'rut':
		        $destination_tipo_identificacion = 'rut';
		    case 'ti':
		        $destination_tipo_identificacion = 'ti';
		        break;
		    default:
		    	$destination_tipo_identificacion = 'otro';
		}
	}
	return $destination_tipo_identificacion;
}

/*
* Mapeo de nivel educativo
*/
function get_nivel_educativo($source_nivel_educativo = ''){
	$destination_nivel_educativo = '';
	if(!empty($source_nivel_educativo)){
		$source_nivel_educativo = strtolower($source_nivel_educativo);
		switch ($source_nivel_educativo) {
		    case '1': // Ninguno
		        $destination_nivel_educativo = '13172';
		        break;
		    case '2': // Primaria
		        $destination_nivel_educativo = '13173';
		        break;
		    case '3': // Secundaria
		        $destination_nivel_educativo = '13175';
		        break;
		    case '4': // Tecnico
		        $destination_nivel_educativo = '13176';
		    case '5': // Tecnologo
		        $destination_nivel_educativo = '13177';
		    case '6': // Profesional
		        $destination_nivel_educativo = '13174';
		    case '7': // Especializacion
		        $destination_nivel_educativo = '13170';
		    case '8': // Maestria
		        $destination_nivel_educativo = '13171';
		    case '9': // Doctorado
		        $destination_nivel_educativo = '13169';
		        break;
		    default:
		    	$destination_nivel_educativo = '13172';
		}
	}
	return $destination_nivel_educativo;
}

function validate_file($type,$name_file){
    $url = 'public://'.$type.'/'.$name_file;
	$url_file = file_exists($url);
    if ($url_file ){
	   return $url;
 	} else {
	  return FALSE;
	}
}

function iso8601($time=false) {
    if ($time === false) $time = time();
    $date = date('Y-m-d\TH:i:s', $time);
    return $date;
}

/* Mapeo Categorías de Productos */
function get_tid_categoria_productos($nom_categoria_productos = ""){
    $mapping = array("81" => 1803, "26" => 1802, "22" => 1798, "23" => 1797, "21" => 1799, "25" => 1800, "24" => 1801, 'Otros' => 14622);

    if (isset($mapping[$nom_categoria_productos])){
       return $mapping[$nom_categoria_productos];
    }
    else{
       return 14622;
    }
}

/* Mapeo Materiales Artesanales */
function get_tid_materiales_artesanales($nom_material_artesanal = ""){
    $mapping = array('1' => 1775, '2' => 1706, '3' => 1725, '4' => 1774, '6' => 1783, '7' => 1714, '8' => 1733, '9' => 1752, '10' => 1771, '11' => 1703, '12' => 1722, '13' => 1741, '14' => 1760, '15' => 1780, '16' => 1711, '17' => 1730, '18' => 1679, '19' => 1678, '20' => 1699, '21' => 1719, '22' => 1739, '23' => 1756, '24' => 1776, '25' => 1702, '26' => 1721, '27' => 1738, '28' => 1758, '29' => 1778, '30' => 1710, '31' => 1729, '32' => 1748, '33' => 1766, '34' => 1786, '35' => 1718, '36' => 1737, '37' => 1755, '38' => 1733, '39' => 1708, '40' => 1727, '41' => 1746, '42' => 1764, '43' => 1784, '44' => 1716, '45' => 1735, '46' => 1753, '47' => 14623, '48' => 14623, '49' => 14623, '50' => 14623, '51' => 1761, '52' => 1781, '53' => 1713, '54' => 1732, '55' => 1750, '56' => 1769, '57' => 1701, '58' => 1720, '59' => 1789, '60' => 1757, '61' => 1777, '62' => 1709, '63' => 1728, '64' => 1747, '65' => 1765, '66' => 1785, '67' => 1717, '68' => 1736, '69' => 1754, '70' => 1774, '71' => 1707, '72' => 1726, '73' => 1745, '74' => 1762, '75' => 1782, '76' => 1715, '77' => 1734, '78' => 1751, '79' => 1770, '80' => 14623, '81' => 1723, '82' => 1742, '83' => 1759, '84' => 1779, '85' => 1712, '86' => 1731, '87' => 1749, '88' => 1767, '101' => 1700, 'Otros' => 14623);

    if (isset($mapping[$nom_material_artesanal])){
       return $mapping[$nom_material_artesanal];
    }
    else{
       return 14623;
    }
}

/* Mapeo Oficio Artesanal */
function get_tid_oficio_artesanal($nom_oficio_artesanal){
	$mapping = array('1' => 1666, '2' => 1680, '3' => 1687, '4' => 1669, '5' => 1693, '6' => 1676, '7' => 1683, '8' => 1690, '9' => 1696, '10' => 1672, '11' => 1679, '12' => 1683, '13' => 1692, '14' => 1668, '15' => 1675, '16' => 1682, '17' => 1695, '18' => 1671, '19' => 1678, '20' => 1667, '21' => 1674, '22' => 1681, '23' => 1688, '24' => 1685, '25' => 1694, '26' => 1670, '27' => 1691, '28' => 1677, '29' => 1684, '30' => 1665, '31' => 1664, '32' => 1673, '33' => 1689, 'Otros' => 1689);

    if (isset($mapping[$nom_oficio_artesanal])){
       return $mapping[$nom_oficio_artesanal];
    }
    else{
       return 1689;
    }
}

/* Mapeo Tecnicas Artesanales */
function get_tid_tecnicas_artesanales($nom_tecnica_artesanal){
	$mapping = array('78' => 1660, '1' => 1586, '2' => 1603, '3' => 1621, '4' => 1639, '5' => 1656, '6' => 1595, '7' => 1612, '8' => 1629, '9' => 1646, '10' => 1663, '11' => 1602, '12' => 1619, '13' => 1636, '14' => 1653, '15' => 1593, '16' => 1610, '17' => 1627, '18' => 1644, '19' => 1661, '20' => 1600, '21' => 1617, '22' => 1634, '23' => 1650, '24' => 1591, '25' => 1608, '26' => 1625, '27' => 1642, '28' => 1659, '29' => 1598, '30' => 1615, '31' => 1632, '32' => 1649, '33' => 1589, '34' => 1606, '35' => 1620, '36' => 1638, '37' => 1655, '38' => 1594, '39' => 1611, '40' => 1628, '41' => 1645, '42' => 1662, '43' => 1601, '44' => 1618, '45' => 1635, '46' => 1652, '47' => 1592, '48' => 1609, '49' => 1626, '50' => 1643, '51' => 1616, '52' => 1599, '53' => 1633, '54' => 1651, '55' => 1590, '56' => 1607, '57' => 1624, '58' => 1641, '59' => 1658, '60' => 1597, '61' => 1614, '62' => 1631, '63' => 1648, '64' => 1588, '65' => 1605, '66' => 1623, '67' => 1637, '68' => 1596, '69' => 1654, '70' => 1587, '71' => 1604, '72' => 1622, '73' => 1640, '74' => 1657, '75' => 1613, '76' => 1630, '77' => 1647, 'Otros' => 1651);

    if (isset($mapping[$nom_tecnica_artesanal])){
       return $mapping[$nom_tecnica_artesanal];
    }
    else{
       return 1651;
    }
}

/* Mapeo id anterior taller a nid taller */
function get_id_anterior_taller($id_anterior){
	$result = db_query("SELECT entity_id FROM field_data_field_id_anterior_taller WHERE bundle = 'taller' AND field_id_anterior_taller_value = :contenido_id LIMIT 1", array(":contenido_id" => $id_anterior))->fetchAssoc();

	if (!empty($result['entity_id'])){
		return $result['entity_id'];
	} else {
		return false;
	}

}

/* Mapeo Tipo de Contacto */
function get_tid_tipo_contactenos($id){
	$mapping = array('10' => 1563, '6' => 1564, '102' => 1566, '7' => 1567, '9' => 1568, '4' => 1569, '8' => 1570, '3' => 1572, '61' => 1574, '2' => 1575, '22' => 1576, '11' => 1577, '41' => 1578, '81' => 1579, '5' => 1580, '101' => 1581, '21' => 1582, '1' => 1583);

    if (isset($mapping[$id])){
       return $mapping[$id];
    }
    else{
       return false;
    }
}
function get_node_by_id_contenido($id_anterior){
	if (empty($id_anterior)){
		return FALSE;

	}
	$result = db_query("SELECT entity_id FROM field_data_field_contenido_id WHERE field_contenido_id_value = :contenido_id LIMIT 1", array(":contenido_id" => $id_anterior))->fetchAssoc();

	if (!empty($result['entity_id'])){
		$node = node_load($result['entity_id']);
		return $node;
	} else {
		return FALSE;
	}

}

function get_tipo_de_asesoria($id){
	$mapping = array('41' => 14731, '42' => 1813, '43' => 13273, '44' => 1815, '1' => 14723, '2' => 1814, '3' => 14724, '4' => 14725, '5' => 14726, '6' => 14727, '7' => 14728, '8' => 14729, '21' => 14730);

    if (isset($mapping[$id])){
       return $mapping[$id];
    }
    else{
       return false;
    }	
}
