<?php
// $Id$
/**
 * @file
 * Privatemsg Views module Views hooks.
 */

/**
 * Implementation of hook_views_data_alter().
 */
function privatemsg_views_views_data_alter(&$data) {  
  // ----------------------------------------------------------------
  // pm_message table
  
  $data['pm_message']['table']['group']  = t('Privatemsg');
  $data['pm_message']['table']['base'] = array(
    'field' => 'mid',
    'title' => t('Private message'),
    'help' => t("Private messages sent between users."),
    'weight' => 10,
  );
  $data['pm_message']['table']['join'] = array(
    'users' => array(
      'left_field' => 'uid',
      'field' => 'author',
    ),
  );
  
  // mid
  $data['pm_message']['mid'] = array(
    'title' => t('Mesage Id'),
    'help' => t('The ID of the private message.'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'subject', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'numeric',
    ),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
  );
  $data['pm_message']['unread_pagenum'] = array(
    'title' => t('First unread page number'),
    'help' => t('Page number of the first unread message in the message thread. If the whole thread is read or returns number of the page containing the message.'),
    'real field' => 'mid',
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_unread_pagenum',
    ),
  );
  $data['pm_message']['message_actions'] = array(
    'title' => t('Message actions'),
    'help' => t('Actions related to private messages e.g. "delete".'),
    'real field' => 'mid',
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_message_actions',
    ),
  );
  
  // subject
  $data['pm_message']['subject'] = array(
    'title' => t('Subject'),
    'help' => t('The subject of the message.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // body
  $data['pm_message']['body'] = array(
    'title' => t('Body'), // The item it appears as on the UI,
    'help' => t('The body of the message.'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'views_handler_field_markup',
      'format' => variable_get('filter_default_format', 1), // The name of the format field or an integer
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // author
  $data['pm_message']['author'] = array(
    'title' => t('Author'),
    'help' => t('Author of the message.'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_user_name',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'click sortable' => TRUE,
    ),
    'relationship' => array(
      'label' => t('author'),
      'help' => t('Private message authors.'),
      'base' => 'users',
      'base field' => 'uid',
    ),
  );

  // sent
  $data['pm_message']['timestamp'] = array(
    'title' => t('Sent date'),
    'help' => t('The date and time when the message was sent.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );
  
  // ----------------------------------------------------------------------
  // pm_index table
  
  $data['pm_index']['table']['group']  = t('Privatemsg');
  $data['pm_index']['table']['join'] = array(
    'pm_message' => array(
      'left_field' => 'mid',
      'field' => 'mid',
      'type' => 'INNER',
    ),
  );

  // thread_id
  $data['pm_index']['thread_id'] = array(
    'title' => t('Message thread'),
    'help' => t('Messages thread ID.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'subject', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'numeric',
    ),
  );
  // Pseudo-field to show participants.
  $data['pm_index']['participants'] = array(
    'title' => t('Participants'),
    'help' => t('Participants.'),
    'real field' => 'thread_id',
    'field' => array(
      'title' => t('Participants'),
      'help' => t('Display user participants.'),
      'handler' => 'privatemsg_views_handler_field_participants',
      'skip base' => 'pm_index',
    ),
  ); 
  // "number of messages in a thread" pseudo-field.
  $data['pm_index']['count_messages'] = array(
    'title' => t('Message count'),
    'help' => t('Number of messages in the thread.'),
    'real field' => 'thread_id',
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_msg_count',
    ),
  );

  // uid
  $data['pm_index']['uid'] = array(
    'title' => t('Participant'),
    'help' => t('Show messages where a user is either an author or a recipient.'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'participant', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'numeric',
    ),
    'relationship' => array(
      'label' => t('participant'),
      'help' => t('Users that are either authors or recipients.'),
      'base' => 'users',
      'base field' => 'uid',
    ),
    'filter' => array(
      'handler' => 'privatemsg_views_handler_filter_participant',
    ),
  );
  
  // is_new
  $data['pm_index']['is_new'] = array(
    'title' => t('New ?'),
    'help' => t('Whether the user has read this message.'),
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_is_new',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'click sortable' => TRUE,
    ),
  );

  // deleted
  $data['pm_index']['deleted'] = array(
    'title' => t('Deleted'),
    'help' => t('When the message was deleted by the participant.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'type' => 'yes-no',
      'label' => t('Deleted'),
      'help' => t('Filter messages depending on their deletion by the participant.')
    ),
  );

  
  
  // ----------------------------------------------------------------------
  // Pseudo tables to emulate aggregate queries.
  
  // first message filter table.
  $data['pmi2']['table']['group']  = t('Privatemsg');
  $data['pmi2']['table']['join'] = array(
    'pm_message' => array(
      'table' => 'pm_index',
      'left_table' => 'pm_index',
      'left_field' => 'uid',
      'field' => 'uid',
      'extra' => 'pm_index.thread_id = pmi2.thread_id AND pm_index.mid > pmi2.mid',
    ),
  );
  $data['pmi2']['thread_id'] = array(
    'title' => t('first message in a thread'),
    'help' => t('Turns private message list into list of first messages.'),
    'filter' => array(
      'handler' => 'privatemsg_views_handler_filter_is_null',
    ),
  );

  // last message filter table.
  $data['pmi3']['table']['group']  = t('Privatemsg');
  $data['pmi3']['table']['join'] = array(
    'pm_message' => array(
      'table' => 'pm_index',
      'left_table' => 'pm_index',
      'left_field' => 'uid',
      'field' => 'uid',
      'extra' => 'pm_index.thread_id = pmi3.thread_id AND pm_index.mid < pmi3.mid',
    ),
  );
  $data['pmi3']['thread_id'] = array(
    'title' => t('last message in a thread'),
    'help' => t('Turns private message list into list of last messages.'),
    'filter' => array(
      'handler' => 'privatemsg_views_handler_filter_is_null',
    ),
  );
  
}

/**
 * Implementation of hook_views_handlers().
 */
function privatemsg_views_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'privatemsg_views') . '/handlers',
    ),
    'handlers' => array(
      'privatemsg_views_handler_field_unread_pagenum' => array(
        'parent' => 'views_handler_field',
      ),
      'privatemsg_views_handler_field_is_new' => array(
        'parent' => 'views_handler_field',
      ),
      'privatemsg_views_handler_field_message_actions' => array(
        'parent' => 'views_handler_field',
      ),
      'privatemsg_views_handler_field_msg_count' => array(
        'parent' => 'views_handler_field_prerender_list',
      ),
      'privatemsg_views_handler_field_participants' => array(
        'parent' => 'views_handler_field',
      ),
      'privatemsg_views_handler_filter_participant' => array(
        'parent' => 'views_handler_filter',
      ),
      'privatemsg_views_handler_filter_is_null' => array(
        'parent' => 'views_handler_filter',
      ),
      
    ),
  );
}


