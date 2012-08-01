<?php

/**
 * @file
 * Webform Addressfield component.
 */

/**
 * Implements _webform_defaults_component().
 *
 * @return
 *   An array defining the default structure of a component.
 */
function _webform_defaults_address() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'mandatory' => 0,
    'pid' => 0,
    'weight' => 0,
    'extra' => array(
      'title_display' => 0,
      'description' => '',
      'private' => FALSE,
    ),
  );
}


/**
 * Implements _webform_theme_component().
 */
function _webform_theme_address() {
  return array(
    'webform_address' => array(
      'render element' => 'element',
    ),
    'webform_display_address' => array(
      'render element' => 'element',
    ),
  );
}

/**
 * Implements hook_edit_component
 */

/** 
 * Generate the form for editing a component.
 *
 * Create a set of form elements to be displayed on the form for editing this
 * component. Use care naming the form items, as this correlates directly to the
 * database schema. The component "Name" and "Description" fields are added to
 * every component type and are not necessary to specify here (although they
 * may be overridden if desired).
 *
 * @param $component
 *   A Webform component array.
 * @return
 *   An array of form items to be displayed on the edit component page
 */
function _webform_edit_address($component) {
  $form = array();

  return $form;
}

/**
 * Implements _webform_render_component().
 */
 
/**
 * Render a Webform component to be part of a form.
 *
 * @param $component
 *   A Webform component array.
 * @param $value
 *   If editing an existing submission or resuming a draft, this will contain
 *   an array of values to be shown instead of the default in the component
 *   configuration. This value will always be an array, keyed numerically for
 *   each value saved in this field.
 */
function _webform_render_address($component, $value = NULL, $filter = TRUE) {
  $element = array(
    '#type' => 'addressfield_container',
    '#title' => $filter ? _webform_filter_xss($component['name']) : $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    // '#required' => $component['mandatory'],
    '#weight' => $component['weight'],
    '#description' => $filter ? _webform_filter_descriptions($component['extra']['description']) : $component['extra']['description'],
    // '#theme' => 'webform_address',
    '#theme_wrappers' => array('webform_element', 'addressfield_container'),
    '#process' => array('webform_expand_address'),
    '#webform_component' => $component,
  );
  
  if ($value) {
    $element['#default_value'] = $value;
  }
  return $element;
}

/**
 * A Form API #process function for Webform address fields.
 */
function webform_expand_address($element) {
  $countries = array('US' => 'United States');

  $address = addressfield_default_values($countries);

  $handlers = array(
   'name' => 'name-full',
   'address' => 'address',
  );
  $context = array(
   'mode' => 'form'
  );

  $element['address'] = addressfield_generate($address, $handlers, $context);
  
  $value = isset($element['#default_value']) ? unserialize($element['#default_value']['address']) : NULL;

  if ($value) {
    foreach($value as $k => $v) {
      $element['address']['#address'][$k] = $v;
    }
  }
  $element['address']['#required'] = $element['#webform_component']['mandatory'];
  
  // Remove the default addressfield country validation & ajaxiness
  // @todo fix this
  $element['address']['country']['#element_validate'] = array();
  $element['address']['country']['#ajax'] = array();

  return $element;
}

/**
 * Implements _webform_submit_component()
 */

/**
 * A hook for changing the input values before saving to the database.
 *
 * Note that Webform will save the result of this function directly into the
 * database.
 *
 * @param $component
 *   A Webform component array.
 * @param $value
 *   The POST data associated with the user input.
 * @return
 *   An array of values to be saved into the database. Note that this should be
 *   a numerically keyed array.
 */
function _webform_submit_address($component, $value) {
  $value['address'] = serialize($value['address']);
  return $value;
}


/**
 * Implements _webform_display_component().
 */

/**
 * Display the result of a submission for a component.
 *
 * The output of this function will be displayed under the "Results" tab then
 * "Submissions". This should output the saved data in some reasonable manner.
 *
 * @param $component
 *   A Webform component array.
 * @param $value
 *   An array of information containing the submission result, directly
 *   correlating to the webform_submitted_data database table schema.
 * @param $format
 *   Either 'html' or 'text'. Defines the format that the content should be
 *   returned as. Make sure that returned content is run through check_plain()
 *   or other filtering functions when returning HTML.
 * @return
 *   A renderable element containing at the very least these properties:
 *    - #title
 *    - #weight
 *    - #component
 *    - #format
 *    - #value
 *   Webform also uses #theme_wrappers to output the end result to the user,
 *   which will properly format the label and content for use within an e-mail
 *   (such as wrapping the text) or as HTML (ensuring consistent output).
 */
 
function _webform_display_address($component, $value, $format = 'html') {
  $address = unserialize($value['address']);
  $handlers = array(
   'name' => 'name-full',
   'address' => 'address',
  );
  $context = array(
   'mode' => 'render'
  );

  // Generate the default webform components

  $element = array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#format' => $format,
    // '#theme' => 'webform_display_address',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#sorted' => TRUE,
    '#webform_component' => $component,
  );
  
  // Render the addressfield component
  $element['address'] = addressfield_generate($address, $handlers, $context);

  return $element;
}

/**
 * Format the text output for this component.
 */
function theme_webform_display_address($variables) {

}

/**
 * Implements _webform_table_component().
 */
function _webform_table_address($component, $value) {
  $address = unserialize($value['address']);
  $handlers = array(
   'address' => 'address',
  );
  $context = array(
   'mode' => 'render'
  );

  $element['address'] = addressfield_generate($address, $handlers, $context);

  return drupal_render($element);
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_address($component, $export_options) {
  
  $address = array('first_name','last_name','thoroughfare','premise','locality','administrative_area','postal_code','
  country');
  $header = array();
  $header[0] = array('');
  $header[1] = array($component['name']);
  $count = 0;
  foreach ($address as $key => $item) {
    // Empty column per sub-field in main header.
    if ($count != 0) {
      $header[0][] = '';
      $header[1][] = '';
    }
    // The value for this option.
    $header[2][] = $item;
    $count++;
  }

  return $header;
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_address($component, $export_options, $value) {
  $address = unserialize($value['address']);

  $return = array();
  foreach ($address as $key => $val) {
    $return[] = $val;
  }
  return $return;
}

