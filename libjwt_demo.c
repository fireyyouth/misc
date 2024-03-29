#include <jwt.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

const char *s = "eyJhbGciOiJSUzM4NCIsInR5cCI6IkpXVCJ9.eyJjaXAiOiIxMjcuMC4wLjEiLCJpYXQiOjE2ODQ4MjcwMzIsImlzcyI6ImNoYWl0aW4vc2FmZWxpbmUtY2UiLCJsYW5nIjoiemgtY24iLCJvc3YiOiJtYWMgMTYuNCIsInJlcyI6IjE2MDAqOTAwIiwidWEiOiJjaHJvbWUgeHh4In0.llA-zJ029IeJOQsKQ6msqppXlvS24V6Z6CS5DaOYJF16aqI026nadYNoAs8VRc0XzIJqCD93AO-3iObrxrlERp_I8S0QEtpGg360XU14pZjiiM6KRHStq_CDy5lumlGTKPq8FWvOpEmenxqndalEMuWLttlEVdseuJX1uF9lG5ZzJGbnQr-JLENfXLnrcMIacZ-Zm2cJe6M_DM7t2kIeegmb-7HSVpPkByNsHrxiDueshrpijUt1zW4giSBsnk8PkLM5AMfYoy0MhZyK5pWZLwK7zKJxrDmG85LQ60J77ZxD-xYoKOp0iblkf5MuuIKAgZBVwdaveyC6SjSD-nTBJITwkXnxBSnsNA04P2r72oDmgce0C0sGvbmn85LoLsumPR5brtxA77lhYUvYXbq1xkMFAk-RayqeTek-mzgGogNVsHbcNZ1qwGn8vgRGLuQ3_-qg_C9QNgesplFyIBHmlfwx9aPsVbAAZGIrlQrvPry7vzOOnMozkLJ7QD68IbZnR9iWo7_EnE1Y0L-F9ixLiWv7UxKVhGp_cwELjuNJz4qXGPWfvgj57yrL7eBi7Pd70xow9GEMrWHs5a3CoW8AzjumYMQjayxDZkdo1ofzZl7tGDmx8x2BV1QlNe_Afe8ClPko5B-JoNCD2uZEYggt58jpV_kANPPqb-BgyBPe8Y0";

const unsigned char *k = "-----BEGIN PUBLIC KEY-----\nMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAzK6t+AggppbKDHPYbA71\nKEPBT7IcJRleJQMgHnil6tG7nARtjucZcXEEZYETBMDDJNj9+FTsVFPxdxPd5uTj\nQARbBd1B+zeptauAljfZ5vGghQPgGN04qsI1iy0iklNmsuTIdl/OXl2MbO6tiaYu\nr2t1iQxpudBB3EQqWWTgsXh8VDEncNsEHZ0K6b/bokb4ZJbyi1RsMLN7a2f7Dyod\nK3EpdtSgJBjVTrIOpgWXZ4euDX0pRFArEzGVMM+31zBohkQfVA6aRRO/37p7xtYb\nf/Ww7MVFVC10hL4ORVMFNvw3UXOS9hAjruBjDUsKuNMZIeWe/QhxC5EGwdhHIY6S\nZhmWZrji2aIcVkVc1qJuq2ol43W4GoK6YyRw+SAlBosyZL+fd6KpHru66u1ks16n\nLkieJnvMo6ITBS/SNYPuszm10sEvDgpgSQGIQooHK4KC4bHtLtUO2uPdz9zghepj\nOQ3NsFq0euRyvWd0a+KMuCi8wJ8takKEcnpMcmL+1e/pcMgH7pGmObj8CD9hWb0u\nmieFO4W1QgoDHfF1Ruwp2OA28tLUT/hu60XFjskjX8XcjaYC+M7uMdFtD3WpqPEC\nMunABd3DhmdBC8in0s2vcC+PTW0oTtCNGcvK3WfJpJ7ocdto5XcjMSE6YFCx+SqD\n7WQNt52MoyPgbIjhWwWIYg0CAwEAAQ==\n-----END PUBLIC KEY-----";

int main() {
    jwt_t *obj;
    int err = jwt_decode(&obj, s, k, strlen(k));
    assert (!err);
    printf("%s\n", jwt_get_grants_json(obj, NULL));
}

