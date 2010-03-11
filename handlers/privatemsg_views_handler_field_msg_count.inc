<?php
// $Id$

/**
 * @file
 * Handler for the "message count" field of private message thread.
 * 
 */
class privatemsg_views_handler_field_msg_count extends views_handler_field_prerender_list {
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
      'not_deleted' => t('Not deleted by current user'),      
      'read' => t('Read and not deleted by current user'),
      'unread' => t('Unread and not deleted by current user '),
      'deleted' => t('Deleted by current user'),
      'all' => t('All messages'),
    );
    $form['count_filter'] = array(
      '#title' => t('Count messages filter'),
      '#description' => t('Please select which messages to count. Atm this works for current user only.'),
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
    global $user;
    
    // Prepare thread list for IN list.
    $threads = array();
    foreach ($values as $value) {
      if (!empty($value->{$this->field_alias})) {
        $threads[] = $value->{$this->field_alias};
      }
    }
    
    $query  = "SELECT pmi.thread_id, COUNT(m.mid) AS msg_count FROM {pm_message} m INNER JOIN {pm_index} pmi ON m.mid = pmi.mid";
    $query .= " WHERE pmi.uid = %d";
    switch ($this->options['include_deleted']) {
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
    }
    $query .= " AND pmi.thread_id IN (" . db_placeholders($threads) . ")";
    $query .= " GROUP BY pmi.thread_id, pmi.uid";
       
    // TODO: Is it possible to link user ID argument here ?
    array_unshift($threads, $user->uid);
    $result = db_query($query, $threads);
    while ($thread = db_fetch_object($result)) {
      $this->items[$thread->thread_id][0]['msg_count'] = $thread->msg_count;
    }
  }
  
  function render_item($count, $item) {
    return $item['msg_count'];
  }
}
