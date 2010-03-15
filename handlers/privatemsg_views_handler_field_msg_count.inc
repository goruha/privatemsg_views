<?php
// $Id$

/**
 * @file
 * Handler for the "message count" field of private message thread.
 * 
 */
class privatemsg_views_handler_field_msg_count extends views_handler_field_prerender_list {
  function init(&$view, $options) {
    parent::init($view, $options);
    $this->additional_fields['uid'] = array('table' => 'pm_index', 'field' => 'uid');
  }
  
  function option_definition() {
    $options = parent::option_definition();
    $options['count_filter'] = array('default' => 'not_deleted');    
    return $options;
  }

  /**
   * Provide "link to profile" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $options = array(
      'not_deleted' => t('Not deleted by participant'),      
      'read' => t('Read and not deleted by participant'),
      'unread' => t('Unread and not deleted by participant'),
      'deleted' => t('Deleted by participant'),
      'all' => t('All messages'),
    );
    $form['count_filter'] = array(
      '#title' => t('Count messages filter'),
      '#description' => t('Please select which messages to count.'),
      '#type' => 'radios',
      '#options' => $options,
      '#default_value' => $this->options['count_filter'],
    );
  }
  
  /**
   * Load message count data in pre_render stage when main data is already 
   * available.
   */
  function pre_render($values) {
    // Prepare thread list for IN list.
    $threads = array();
    foreach ($values as $value) {
      if (!empty($value->{$this->field_alias})) {
        $threads[] = $value->{$this->field_alias};
      }
    }
    $query  = "SELECT pmi.thread_id, pmi.uid, COUNT(pmi.mid) AS msg_count FROM {pm_index} pmi";
    $query .= " WHERE pmi.thread_id IN (" . db_placeholders($threads) . ")";
    switch ($this->options['count_filter']) {
      case 'not_deleted':
        $query .= " AND pmi.deleted = 0";
        break;      
      case 'read':
        $query .= " AND pmi.is_new = 0 AND pmi.deleted = 0";
        break;
      case 'unread':
        $query .= " AND pmi.is_new = 1 AND pmi.deleted = 0";
        break;
      case 'deleted':
        $query .= " AND pmi.deleted > 0";
        break;
      case 'all':
        // no filtering
    }
    $query .= " GROUP BY pmi.thread_id, pmi.uid";
    $result = db_query($query, $threads);
    while ($thread = db_fetch_object($result)) {
      // Add uid key so every participant gets own msg count.
      $this->items[$thread->thread_id][$thread->uid][0]['msg_count'] = $thread->msg_count;
    }
  }
  
  function get_items($values) {
    $uid = $values->{$this->aliases['uid']};
    $thread_id = $values->{$this->field_alias};
    if (!empty($this->items[$thread_id][$uid])) {
      return $this->items[$thread_id][$uid];
    }
    return array(0 => array('msg_count' => 0));
  }
  
  function render_item($count, $item) {
    return $item['msg_count'];
  }
}
