<?php
/**
 * @file
 * Contains LTIToolProviderOAuthDataStore.
 */
$library = libraries_load('oauth');
if ($library['loaded'] == FALSE) {
  drupal_set_message(t('Missing OAuth library: @error', array('@error' => $library['error'])), 'error');
}
else {
  require_once($library['library path'] . '/OAuth.php');

  class LTIToolProviderOAuthDataStore extends OAuthDataStore {
    function lookup_consumer($consumer_key) {
      $query = new EntityFieldQuery();    
      $result = $query->entityCondition('entity_type', 'ltitp_consumer_entity')
      ->propertyCondition('ltitp_consumer_entity_key', $consumer_key, '=')
      ->addMetaData('account', user_load(1))
      ->execute();    
      if (isset($result['ltitp_consumer_entity'])) {
        $consumer_ids = array_keys($result['ltitp_consumer_entity']);
        $consumers = entity_load('ltitp_consumer_entity', $consumer_ids);
        $consumer_entity = reset($consumers);
        $consumer = new OAuthConsumer($consumer_key, $consumer_entity->ltitp_consumer_entity_secret, NULL);
      }
      else {
        $consumer = new OAuthConsumer($consumer_key, NULL, NULL);
      }
      return $consumer;
    }

    function lookup_token($consumer, $token_type, $token) {
      return new OAuthToken($consumer, '');
    }

    function lookup_nonce($consumer, $token, $nonce, $timestamp) {
      return NULL;
    }

    function new_request_token($consumer, $callback = NULL) {
      // return a new token attached to this consumer
      return NULL;
    }

    function new_access_token($token, $consumer, $verifier = NULL) {
      // return a new access token attached to this consumer
      // for the user associated with this token if the request token
      // is authorized
      // should also invalidate the request token
      return NULL;
    }
  }
}