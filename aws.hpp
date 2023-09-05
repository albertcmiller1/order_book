#include <aws/core/client/ClientConfiguration.h>
#include <aws/dynamodb/model/AttributeValue.h>

namespace AwsDoc {
    namespace DynamoDB { 
        bool putItem(const Aws::String &table_name,
                        const Aws::String &transaction_id_key,
                        const Aws::String &transaction_id_value,
                        const Aws::String &date_key,
                        const Aws::String &date_value,
                        const Aws::String &price_key,
                        const Aws::String &price_value,
                        const Aws::String &quantity_key,
                        const Aws::String &quantity_value,
                        const Aws::String &ticker_key,
                        const Aws::String &ticker_value,
                        const Aws::String &transaction_type_key,
                        const Aws::String &transaction_type_value,
                        const Aws::String &user_id_key,
                        const Aws::String &user_id_value,
                        const Aws::Client::ClientConfiguration &clientConfiguration);
    } 
}