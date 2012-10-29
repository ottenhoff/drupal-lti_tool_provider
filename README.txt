The LTI Tool Provider module (lti_tool_provider) allows a Drupal site to serve as a Learning Tools Interoperability (LTI) Tool
in any Learning Management System (LMS) supporting the LTI standard.
Example LTI compliant LMSs are Blackboard Learn, Moodle and Sakai.

Installation
Download and place the module in your sites/all/modules or other suitable directory. 
Download the module dependencies, http://drupal.org/project/entity and http://drupal.org/project/libraries
Download the OAuth.php library file http://oauth.googlecode.com/svn/code/php/OAuth.php and place it in sites/all/libraries/oauth/
Enable the LTI Tool Provider module and optionally the LTI Tool Provider Outcomes and LTI Tool Provider Memberships submodules.

Administrative settings admin/lti_tool_provider

Configure Tool Consumers
Multiple Tool Consumers (LMS) are supported. Each should have a unique Key and Secret which must match in the configuration
of both the consumer and provider. An account domain setting is available which will cause all accounts provisioned
from the corresponding consumer to have @domain appended to the account name. Thus preventing account name conflicts
between different tool consumers.

Configure User Mapping
Accounts
LTI Tool Provider auto provisions Drupal user accounts using the LTI provided user_id, optionally appended with the consumer
account domain, as the Drupal account name. It is possible to map the LTI provided lis_person_name_full, lis_person_name_family 
and lis_person_name_given to any text field on the Drupal user entity.
For example, create a field_fullname in admin/config/people/accounts/fields and have LTI Tool Provider map lis_person_name_full
to field_fullname, then use the Drupal Realname module to use field_fullname for user name display.
If provided, the lis_person_contact_email_primary is mapped to the account mail field otherwise a dummy email 
user_id@domain.example.com is used.

Drupal Role Mapping
The roles provided from the LMS can be mapped to any Drupal global roles. Note: If you map Administrator to Administrator, the
admin of the LMS becomes an admin of your Drupal site.

Course to Group Mapping
If you have the Drupal Organic Groups installed (7.x-2.x only, 7.x-1.x not supported), LTI Tool Provider will allow you to map
LMS courses identified by the LMS context_label, to a group type i.e. bundle. The context_label can be mapped to the group title
or any other text field available on the group bundle.
Optionally groups may be auto-provisioned when a user with the required mapped role, performs an LTI launch. This user becomes
the Group owner. If a field other than title is chosen for context_label, the context_title is mapped to the group title when 
provisioning.

Group Roles
If Course/Group mapping is configured, you may map any of the LMS roles to Group Roles defined on the selected OG Group Bundle.

Launch URL
The LMS must be configured to launch the Drupal site with the path "lti". e.g. https://www.example.com/lti
NOTE: the LTI specification requires the use of TLS for security in the OAuth based LTI launch. You should use the Secure Pages 
module or other method to support https in your Drupal site.

Drupal Landing Page
Upon launch, LMS users will be directed to the Drupal site home page unless Course to Group mapping is enabled, in which case the
user is directed to the group node page for the group which corresponds to the LTI context_id.

LTI Return URL
If it exists, the launch_presentation_return_url is provided as a user menu item titled with the custom_return_label or "Return to LMS".
Returning to the LMS will log out the user from Drupal.

Home menu
A home menu item is provided as a user menu item titled with custom_destination_label or resource_link_title or Home.

Custom Launch Parameters
Some LMS LTI implementations allow the configuration of custom launch parameters. The following custom parameters are processed
by LTI Tool Provider.

custom_destination defines a landing page path, if provided it will override the default (home page or group node) landing page.
custom_title defines an alternate course/group title, if provided, it will be used as the group title when provisioning groups.